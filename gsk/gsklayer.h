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

#ifndef __GSK_LAYER_H__
#define __GSK_LAYER_H__

#if !defined (__GSK_H_INSIDE__) && !defined (GSK_COMPILATION)
#error "Only <gsk/gsk.h> can be included directly."
#endif

#include <gsk/gsktypes.h>
#include <gsk/gsklayeriter.h>

G_BEGIN_DECLS

#define GSK_TYPE_LAYER (gsk_layer_get_type ())

GDK_AVAILABLE_IN_3_18
G_DECLARE_DERIVABLE_TYPE (GskLayer, gsk_layer, GSK, LAYER, GInitiallyUnowned)

/**
 * GskLayerClass:
 *
 * The `GskLayerClass` structure contains only private data.
 *
 * Since: 3.16
 */
struct _GskLayerClass
{
  /*< private >*/
  GInitiallyUnownedClass parent_class;

  /*< public >*/
  gboolean (* queue_redraw) (GskLayer *self,
                             GskLayer *origin);
  gboolean (* queue_relayout) (GskLayer *self,
                               GskLayer *origin);

  void (* get_preferred_size) (GskLayer *self,
                               graphene_size_t *size);

  void (* child_added) (GskLayer *self,
                        GskLayer *child);
  void (* child_removed) (GskLayer *self,
                          GskLayer *child);

  void (* layout_children) (GskLayer *self);

  gboolean (* draw) (GskLayer *self,
                     cairo_t *cr);

  /*< private >*/
  gpointer _padding[16];
};

GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_new                           (void);

/* Scene graph */

GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_get_parent                    (GskLayer *self);
GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_get_next_sibling              (GskLayer *self);
GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_get_prev_sibling              (GskLayer *self);
GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_get_first_child               (GskLayer *self);
GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_get_last_child                (GskLayer *self);
GDK_AVAILABLE_IN_3_18
guint           gsk_layer_get_n_children                (GskLayer *self);
GDK_AVAILABLE_IN_3_18
GList *         gsk_layer_get_children                  (GskLayer *self);

GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_add_child                     (GskLayer *self,
                                                         GskLayer *child);
GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_remove_child                  (GskLayer *self,
                                                         GskLayer *child);
GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_insert_child_at_index         (GskLayer *self,
                                                         GskLayer *child,
                                                         int       index_);
GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_insert_child_after            (GskLayer *self,
                                                         GskLayer *child,
                                                         GskLayer *sibling);
GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_insert_child_before           (GskLayer *self,
                                                         GskLayer *child,
                                                         GskLayer *sibling);
GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_replace_child                 (GskLayer *self,
                                                         GskLayer *old_child,
                                                         GskLayer *new_child);
GDK_AVAILABLE_IN_3_18
GskLayer *      gsk_layer_get_child_at_index            (GskLayer *self,
                                                         int       index_);
GDK_AVAILABLE_IN_3_18
gboolean        gsk_layer_contains                      (GskLayer *self,
                                                         GskLayer *descendant);

GDK_AVAILABLE_IN_3_18
void            gsk_layer_queue_redraw                  (GskLayer *self);
GDK_AVAILABLE_IN_3_18
gboolean        gsk_layer_needs_redraw                  (GskLayer *self);
GDK_AVAILABLE_IN_3_18
void            gsk_layer_queue_relayout                (GskLayer *self);
GDK_AVAILABLE_IN_3_18
gboolean        gsk_layer_needs_relayout                (GskLayer *self);

GDK_AVAILABLE_IN_3_18
void            gsk_layer_set_hidden                    (GskLayer *self,
                                                         gboolean  hidden);
GDK_AVAILABLE_IN_3_18
gboolean        gsk_layer_get_hidden                    (GskLayer *self);
GDK_AVAILABLE_IN_3_18
void            gsk_layer_set_opacity                   (GskLayer *self,
                                                         double    opacity);
GDK_AVAILABLE_IN_3_18
double          gsk_layer_get_opacity                   (GskLayer *self);
GDK_AVAILABLE_IN_3_18
void            gsk_layer_set_background_color          (GskLayer *self,
                                                         const GdkRGBA *bg_color);
GDK_AVAILABLE_IN_3_18
void            gsk_layer_get_background_color          (GskLayer *self,
                                                         GdkRGBA *bg_color);
GDK_AVAILABLE_IN_3_18
void            gsk_layer_set_clip                      (GskLayer *self,
                                                         const graphene_rect_t *clip);
GDK_AVAILABLE_IN_3_18
void            gsk_layer_get_clip                      (GskLayer *self,
                                                         graphene_rect_t *clip);

/* Geometry */

GDK_AVAILABLE_IN_3_18
void                    gsk_layer_set_frame                     (GskLayer               *self,
                                                                 const graphene_rect_t  *frame);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_get_frame                     (GskLayer               *self,
                                                                 graphene_rect_t        *frame);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_set_pivot_point               (GskLayer               *self,
                                                                 const graphene_point_t *point);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_get_pivot_point               (GskLayer               *self,
                                                                 graphene_point_t       *point);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_set_bounds                    (GskLayer               *self,
                                                                 const graphene_rect_t  *bounds);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_get_bounds                    (GskLayer               *self,
                                                                 graphene_rect_t        *bounds);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_set_position                  (GskLayer               *self,
                                                                 const graphene_point_t *position);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_get_position                  (GskLayer               *self,
                                                                 graphene_point_t       *position);

GDK_AVAILABLE_IN_3_18
void                    gsk_layer_get_preferred_size            (GskLayer               *self,
                                                                 graphene_size_t        *size);

/* Transformations */

GDK_AVAILABLE_IN_3_18
void                    gsk_layer_set_rotation                  (GskLayer               *self,
                                                                 const graphene_euler_t *rotation);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_get_rotation                  (GskLayer               *self,
                                                                 graphene_euler_t       *rotation);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_set_scale                     (GskLayer               *self,
                                                                 float                   scale_x,
                                                                 float                   scale_y);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_get_scale                     (GskLayer               *self,
                                                                 float                  *scale_x,
                                                                 float                  *scale_y);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_set_translation               (GskLayer               *self,
                                                                 const graphene_point3d_t *translation);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_get_translation               (GskLayer                 *self,
                                                                 graphene_point3d_t       *translation);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_set_transform                 (GskLayer                 *self,
                                                                 const graphene_matrix_t  *transform);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_get_transform                 (GskLayer                 *self,
                                                                 graphene_matrix_t        *transform);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_set_child_transform           (GskLayer                 *self,
                                                                 const graphene_matrix_t  *transform);
GDK_AVAILABLE_IN_3_18
void                    gsk_layer_get_child_transform           (GskLayer                 *self,
                                                                 graphene_matrix_t        *transform);

GDK_AVAILABLE_IN_3_18
void                    gsk_layer_set_frame_clock               (GskLayer                 *self,
                                                                 GdkFrameClock            *frame_clock);

G_END_DECLS

#endif /* __GSK_LAYER_H__ */
