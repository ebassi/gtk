/* GSK - The GTK scene graph toolkit
 * Copyright 2015  Emmanuele Bassi 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:gskrenderer
 * @Short_description: Renders the scene graph
 * @Title: GskRenderer
 *
 * #GskRenderer is used to render a #GskLayer scene on a #cairo_surface_t.
 */

#include "config.h"

#include "gskrenderer.h"

#include "gskdebug.h"
#include "gsklayerprivate.h"

#include <cairo.h>

struct _GskRenderer
{
  GObject parent_instance;

  GskLayer *root;

  graphene_rect_t viewport;

  graphene_matrix_t modelview;
  graphene_matrix_t projection;
  graphene_matrix_t mvp;

  int scale_factor;

  guint needs_matrix_update : 1;
};

struct _GskRendererClass
{
  GObjectClass parent_class;
};

G_DEFINE_TYPE (GskRenderer, gsk_renderer, G_TYPE_OBJECT)

enum {
  PROP_ROOT = 1,
  N_PROPS
};

static GParamSpec *obj_props[N_PROPS];

static void
gsk_renderer_finalize (GObject *gobject)
{
  GskRenderer *self = GSK_RENDERER (gobject);

  g_clear_object (&self->root);

  G_OBJECT_CLASS (gsk_renderer_parent_class)->finalize (gobject);
}

static void
gsk_renderer_set_property (GObject *gobject,
                           guint prop_id,
                           const GValue *value,
                           GParamSpec *pspec)
{
  GskRenderer *self = GSK_RENDERER (gobject);

  switch (prop_id)
    {
    case PROP_ROOT:
      self->root = g_value_dup_object (value);
      break;

    default:
      break;
    }
}

static void
gsk_renderer_get_property (GObject *gobject,
                           guint prop_id,
                           GValue *value,
                           GParamSpec *pspec)
{
  GskRenderer *self = GSK_RENDERER (gobject);

  switch (prop_id)
    {
    case PROP_ROOT:
      g_value_set_object (value, self->root);
      break;

    default:
      break;
    }
}

static void
gsk_renderer_class_init (GskRendererClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gsk_renderer_set_property;
  gobject_class->get_property = gsk_renderer_get_property;
  gobject_class->finalize = gsk_renderer_finalize;

  /**
   * GskRenderer:root:
   *
   * The root #GskLayer to render. All descendants of the root layer
   * will be traversed and rendered on the surface associated to the
   * #GskRenderer instance.
   *
   * Since: 3.16
   */
  obj_props[PROP_ROOT] =
    g_param_spec_object ("root", "Root", "Root layer to render",
                         GSK_TYPE_LAYER,
                         G_PARAM_READWRITE |
                         G_PARAM_CONSTRUCT_ONLY |
                         G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, N_PROPS, obj_props);
}

static void
gsk_renderer_init (GskRenderer *self)
{
  graphene_matrix_init_identity (&self->modelview);
  graphene_matrix_init_identity (&self->projection);
  graphene_matrix_init_identity (&self->mvp);

  self->scale_factor = 1;

  self->needs_matrix_update = FALSE;
}

GskRenderer *
gsk_renderer_new (GskLayer *root)
{
  g_return_val_if_fail (GSK_IS_LAYER (root), NULL);

  return g_object_new (GSK_TYPE_RENDERER, "root", root, NULL);
}

void
gsk_renderer_set_viewport (GskRenderer *renderer,
                           const graphene_rect_t *viewport)
{
  g_return_if_fail (GSK_IS_RENDERER (renderer));
  g_return_if_fail (viewport != NULL);

  graphene_rect_init_from_rect (&renderer->viewport, viewport);
}

void
gsk_renderer_set_modelview (GskRenderer *renderer,
                            const graphene_matrix_t *modelview)
{
  g_return_if_fail (GSK_IS_RENDERER (renderer));
  g_return_if_fail (modelview != NULL);

  graphene_matrix_init_from_matrix (&renderer->modelview, modelview);

  renderer->needs_matrix_update = TRUE;
}

void
gsk_renderer_set_projection (GskRenderer *renderer,
                             const graphene_matrix_t *projection)
{
  g_return_if_fail (GSK_IS_RENDERER (renderer));
  g_return_if_fail (projection != NULL);

  graphene_matrix_init_from_matrix (&renderer->projection, projection);

  renderer->needs_matrix_update = TRUE;
}

void
gsk_renderer_set_scale_factor (GskRenderer *renderer,
                               int          scale)
{
  g_return_if_fail (GSK_IS_RENDERER (renderer));
  g_return_if_fail (scale > 0);

  renderer->scale_factor = scale;
}

static void
gsk_renderer_maybe_update_matrices (GskRenderer *renderer)
{
  if (!renderer->needs_matrix_update)
    return;

  graphene_matrix_multiply (&renderer->modelview,
                            &renderer->projection,
                            &renderer->mvp);
}

static void
gsk_renderer_render_layer (GskRenderer *renderer,
                           GskLayer    *layer,
                           cairo_t     *cr)
{
  GskLayerState *state = gsk_layer_get_state (layer);
  const GeometryInfo *ginfo;
  const RenderInfo *rinfo;
  graphene_rect_t frame;
  GskLayerIter iter;
  GskLayer *child;
  const graphene_matrix_t *mv;
  graphene_point_t pivot;
  cairo_matrix_t ctm;
  cairo_matrix_t matrix;
  guint8 opacity;

  cairo_save (cr);

  ginfo = gsk_layer_state_peek_geometry_info (state);
  rinfo = gsk_layer_state_peek_render_info (state);
  opacity = CLAMP (rinfo->opacity * 255, 0, 255);

  if (opacity != 255)
    {
      GSK_NOTE (RENDER, g_print ("Pushing group for opacity of %.2f\n", rinfo->opacity));
      cairo_push_group (cr);
    }

  gsk_layer_get_frame (layer, &frame);
  GSK_NOTE (RENDER, g_print ("Rendering layer { %.2f, %.2f x %.2f - %.2f }\n",
                             frame.origin.x, frame.origin.y,
                             frame.size.width, frame.size.height));

  pivot.x = ginfo->pivot_point.x * frame.size.width;
  pivot.y = ginfo->pivot_point.y * frame.size.height;
  cairo_matrix_init_translate (&matrix, frame.origin.x + pivot.x, frame.origin.y + pivot.y);

  /* If the layer only has a 2D modelview we can just
   * draw it on the current surface...
   */
  mv = gsk_layer_get_modelview (layer);
  if (graphene_matrix_to_2d (mv,
                             &ctm.xx, &ctm.yx,
                             &ctm.xy, &ctm.yy,
                             &ctm.x0, &ctm.y0))
    {
      cairo_matrix_multiply (&matrix, &ctm, &matrix);
    }
  else
    {
      /* Otherwise we break up the draw; we take the layer, draw it
       * it inside a separate surface, then use GL to render the
       * surface use the modelview matrix; then we take the render
       * buffer and draw it on top of the previous source.
       */
      g_printerr ("Not a 2D transformation\n");
    }

  cairo_matrix_translate (&matrix, -pivot.x, -pivot.y);

  cairo_transform (cr, &matrix);

  gsk_layer_draw (layer, cr);

  gsk_layer_iter_init (&iter, layer);
  while (gsk_layer_iter_next (&iter, &child))
    gsk_renderer_render_layer (renderer, child, cr);

  if (opacity != 255)
    {
      cairo_pop_group_to_source (cr);
      cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
      cairo_paint_with_alpha (cr, rinfo->opacity);
    }

  cairo_restore (cr);

#ifdef G_ENABLE_DEBUG
  if (gsk_get_debug_flags () & GSK_DEBUG_GEOMETRY)
    {
      float x = frame.origin.x + pivot.x;
      float y = frame.origin.y + pivot.y;

      /* Draw the frame */
      cairo_save (cr);
      cairo_set_line_width (cr, 1.0);
      cairo_set_source_rgba (cr, 1, 0, 0, 0.25);
      cairo_rectangle (cr, frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
      cairo_stroke (cr);

      /* Draw the pivot point */
      cairo_set_source_rgba (cr, 0, 0, 1, 0.25);
      cairo_move_to (cr, x, y);
      cairo_arc (cr, x, y, 3, 0, G_PI * 2);
      cairo_fill (cr);
      cairo_restore (cr);
    }
#endif /* G_ENABLE_DEBUG */
}

void
gsk_renderer_render (GskRenderer *renderer,
                     cairo_t     *cr,
                     int          width,
                     int          height)
{
  cairo_surface_t *surface;
  cairo_t *inner_cr;

  g_return_if_fail (GSK_IS_RENDERER (renderer));

  surface = cairo_surface_create_similar (cairo_get_target (cr),
                                          CAIRO_CONTENT_COLOR_ALPHA,
                                          width, height);
  inner_cr = cairo_create (surface);

  gsk_renderer_maybe_update_matrices (renderer);
  gsk_renderer_render_layer (renderer, renderer->root, inner_cr);

  cairo_destroy (inner_cr);

  cairo_save (cr);

  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);

  cairo_restore (cr);

  cairo_surface_destroy (surface);
}
