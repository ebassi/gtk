#ifndef __GSK_LAYER_PRIVATE_H__
#define __GSK_LAYER_PRIVATE_H__

#include "gsklayer.h"

G_BEGIN_DECLS

typedef struct _GskLayerState   GskLayerState;
typedef struct _GeometryInfo    GeometryInfo;
typedef struct _TransformInfo   TransformInfo;
typedef struct _RenderInfo      RenderInfo;

/*< private >
 * GeometryInfo:
 * @bounds: The bounds of the layer, expressed as a rectangle
 *   with the origin in the top left corner of the layer
 * @pivot_point: The normalized point, in layer coordinate
 *   space, used as the center for layout and transformations
 * @position: The coordinates, in parent-relative coordinate
 *   space, of the pivot point
 *
 * The geometry of the layer.
 *
 *           20            80
 *     +======|=============|======+
 *     |                           |
 *  10 -      +-------------+      |
 *     |      |      |      | h    |
 *     |      |      |      | e    |
 *     |      |------P------| i    |
 *     |      |      |      | g    |
 *     |      |      |      | h    |
 *  50 -      +-------------+ t    |
 *     |      ^^^^ width ^^^^      |
 *     +===========================+
 *
 * The composite of all the geometry information is called "frame"
 * and it's re-computed on request; the frame is the bounds rectangle
 * with an origin in parent-relative coordinates. Each parent uses
 * the frame of its children layers when drawing them.
 *
 * Given:
 *   - a pivot point set at (0.5, 0.5)
 *   - a bounds rectangle is: (0, 0, 60, 40)
 *   - a position P set at (100, 60)
 *
 * Then:
 *   - the frame rectangle is: (20, 10, 60, 40)
 *
 * Where:
 *   - frame.origin.x = position.x - (bounds.origin.x + bounds.size.width) * pivot.x
 *   - frame.origin.y = position.y - (bounds.origin.y + bounds.size.height) * pivot.y
 *   - frame.size.width = bounds.origin.x + bounds.size.width
 *   - frame.size.height = bounds.origin.y + bounds.size.height
 *
 * If we set the pivot point to (0, 0), i.e. the top-left corner of
 * the layer, and we don't want to change the frame rectangle, we
 * need to also set the position at (20, 10); similarly, if we set
 * the pivot point to be (1, 1), i.e. the bottom-right corner of the
 * layer, we need to set the position at (80, 50).
 */
struct _GeometryInfo
{
  graphene_rect_t bounds;

  graphene_point_t pivot_point;
  graphene_point_t position;
};

/*< private >
 * TransformInfo:
 * @rotation: the decomposed rotations, in Euler angles
 * @scale: the decomposed scaling factors
 * @translate: the decomposed translation factors
 * @pivot_z: the Z component of the pivot point
 * @transform: the additional transformation matrix for the layer
 * @child_transform: the transformation matrix applied before
 *   rendering each child
 * @modelview: the transformation matrix applied to a layer
 * @inverse: the inverse of @modelview, used for hit testing
 * @needs_modelview_update: whether the @transform needs to be updated
 * @needs_inverse_update: whether the @inverse needs to be updated
 *
 * The transformations applied to a layer.
 *
 * The transformations are stored in two ways: decomposed, for
 * writing simple accessors; and composed, for use when rendering
 * and hit testing.
 */
struct _TransformInfo
{
  graphene_euler_t rotation;
  graphene_vec3_t scale;
  graphene_point3d_t translate;

  float pivot_z;

  graphene_matrix_t transform;
  graphene_matrix_t child_transform;

  graphene_matrix_t modelview;
  graphene_matrix_t inverse;

  guint needs_modelview_update : 1;
  guint needs_inverse_update : 1;
};

/*< private >
 * RenderInfo:
 * @background_color: the color used to render the background
 * @opacity: the opacity to be applied to the layer
 *
 * The rendering state applied to a layer when drawing.
 */
struct _RenderInfo
{
  GdkRGBA background_color;

  double opacity;

  graphene_rect_t clip;

  guint use_clip : 1;
};

/*< private >
 * GskLayerState:
 * @geometry_info: the geometry of the layer
 * @transform_info: the transformations of the layer
 * @render_info: the content of the layer
 *
 * A structure representing the state of a #GskLayer.
 */
struct _GskLayerState
{
  GeometryInfo *geometry_info;
  TransformInfo *transform_info;
  RenderInfo *render_info;
};

gint64                  gsk_layer_get_age                       (GskLayer *layer);
GskLayerState *         gsk_layer_get_state                     (GskLayer *layer);
const char *            gsk_layer_get_debug_name                (GskLayer *layer);
gboolean                gsk_layer_draw                          (GskLayer *layer,
                                                                 cairo_t  *cr);

const graphene_matrix_t *gsk_layer_get_modelview                (GskLayer *layer);
const graphene_matrix_t *gsk_layer_get_inverse_modelview        (GskLayer *layer);

GskLayerState *         gsk_layer_state_copy                    (GskLayerState *state);
void                    gsk_layer_state_free                    (GskLayerState *state);
void                    gsk_layer_state_clear                   (GskLayerState *state);

const GeometryInfo *    gsk_layer_state_peek_geometry_info      (GskLayerState *state);
GeometryInfo *          gsk_layer_state_get_geometry_info       (GskLayerState *state);
const TransformInfo *   gsk_layer_state_peek_transform_info     (GskLayerState *state);
TransformInfo *         gsk_layer_state_get_transform_info      (GskLayerState *state);
const RenderInfo *      gsk_layer_state_peek_render_info        (GskLayerState *state);
RenderInfo *            gsk_layer_state_get_render_info         (GskLayerState *state);

void                    gsk_layer_state_get_frame               (GskLayerState *state,
                                                                 graphene_rect_t *frame);

G_END_DECLS

#endif /* __GSK_LAYER_PRIVATE_H__ */
