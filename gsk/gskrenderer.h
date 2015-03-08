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

#ifndef __GSK_RENDERER_H__
#define __GSK_RENDERER_H__

#if !defined (__GSK_H_INSIDE__) && !defined (GSK_COMPILATION)
#error "Only <gsk/gsk.h> can be included directly."
#endif

#include <gsk/gsktypes.h>

G_BEGIN_DECLS

#define GSK_TYPE_RENDERER (gsk_renderer_get_type ())

GDK_AVAILABLE_IN_3_18
G_DECLARE_FINAL_TYPE (GskRenderer, gsk_renderer, GSK, RENDERER, GObject)

GDK_AVAILABLE_IN_3_18
GskRenderer *   gsk_renderer_new                (GskLayer                *root);

GDK_AVAILABLE_IN_3_18
void            gsk_renderer_set_modelview      (GskRenderer             *renderer,
                                                 const graphene_matrix_t *mv);
GDK_AVAILABLE_IN_3_18
void            gsk_renderer_set_projection     (GskRenderer             *renderer,
                                                 const graphene_matrix_t *projection);
GDK_AVAILABLE_IN_3_18
void            gsk_renderer_set_viewport       (GskRenderer             *renderer,
                                                 const graphene_rect_t   *viewport);
GDK_AVAILABLE_IN_3_18
void            gsk_renderer_set_clear_color    (GskRenderer             *renderer,
                                                 const GdkRGBA           *color);
GDK_AVAILABLE_IN_3_18
void            gsk_renderer_set_scale_factor   (GskRenderer             *renderer,
                                                 int                      factor);

GDK_AVAILABLE_IN_3_18
void            gsk_renderer_render             (GskRenderer             *renderer,
                                                 cairo_t                 *cr,
                                                 int                      width,
                                                 int                      height);

G_END_DECLS

#endif /* __GSK_RENDERER_H__ */
