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

#ifndef __GSK_LAYER_CONTENT_H__
#define __GSK_LAYER_CONTENT_H__

#if !defined (__GSK_H_INSIDE__) && !defined (GSK_COMPILATION)
#error "Only <gsk/gsk.h> can be included directly."
#endif

#include <gsk/gsktypes.h>

G_BEGIN_DECLS

#define GSK_TYPE_LAYER_CONTENT (gsk_layer_content_get_type ())

GDK_AVAILABLE_IN_3_18
G_DECLARE_INTERFACE (GskLayerContent, gsk_layer_content, GSK, LAYER_CONTENT, GObject)

struct _GskLayerContentInterface
{
  GTypeInterface g_iface;

  void     (* draw) (GskLayerContent *content,
                     GskLayer        *layer,
                     cairo_t         *cr);

  gboolean (* needs_redraw) (GskLayerContent *content);

  gboolean (* get_preferred_size) (GskLayerContent *content,
                                   graphene_size_t *size);
};

G_END_DECLS

#endif /* __GSK_LAYER_CONTENT_H__ */
