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

#ifndef __GSK_LAYER_ITER_H__
#define __GSK_LAYER_ITER_H__

#if !defined (__GSK_H_INSIDE__) && !defined (GSK_COMPILATION)
#error "Only <gsk/gsk.h> can be included directly."
#endif

#include <gsk/gsktypes.h>

G_BEGIN_DECLS

#define GSK_TYPE_LAYER_ITER (gsk_layer_iter_get_type ())

/**
 * GskLayerIter:
 *
 * A structure that allows to efficiently iterate over a branch
 * of the scene graph.
 *
 * You should typicall place a `GskLayerIter` structure on the
 * stack.
 *
 * The contents of the `GskLayerIter` structure are private and
 * should only be accessed using the provided API.
 *
 * Since: 3.18
 */
struct _GskLayerIter
{
  /*< private >*/
  GSK_PRIVATE_FIELD (GskLayer *, root);
  GSK_PRIVATE_FIELD (GskLayer *, current);
  GSK_PRIVATE_FIELD (gint64, root_age);
  GSK_PRIVATE_FIELD (gpointer, padding1);
  GSK_PRIVATE_FIELD (gpointer, padding2);
};

GDK_AVAILABLE_IN_3_18
GType gsk_layer_iter_get_type (void) G_GNUC_CONST;

GDK_AVAILABLE_IN_3_18
void            gsk_layer_iter_init     (GskLayerIter  *iter,
                                         GskLayer      *root);
GDK_AVAILABLE_IN_3_18
gboolean        gsk_layer_iter_is_valid (GskLayerIter  *iter);
GDK_AVAILABLE_IN_3_18
gboolean        gsk_layer_iter_next     (GskLayerIter  *iter,
                                         GskLayer     **child);
GDK_AVAILABLE_IN_3_18
gboolean        gsk_layer_iter_prev     (GskLayerIter  *iter,
                                         GskLayer     **child);
GDK_AVAILABLE_IN_3_18
void            gsk_layer_iter_remove   (GskLayerIter  *iter);

G_END_DECLS

#endif /* __GSK_LAYER_ITER_H__ */
