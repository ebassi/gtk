/* GSK - The GTK scene graph toolkit
 * Copyright 2015  Emmanuele Bassi 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "gsklayercontentprivate.h"
#include "gsklayerprivate.h"

G_DEFINE_INTERFACE (GskLayerContent, gsk_layer_content, G_TYPE_OBJECT)

static void
gsk_layer_content_real_draw (GskLayerContent *self,
                             GskLayer        *layer,
                             cairo_t         *cr)
{
}

static gboolean
gsk_layer_content_real_get_preferred_size (GskLayerContent *self,
                                           graphene_size_t *size)
{
  return FALSE;
}

static gboolean
gsk_layer_content_real_needs_redraw (GskLayerContent *self)
{
  return FALSE;
}

static void
gsk_layer_content_default_init (GskLayerContentInterface *iface)
{
  iface->get_preferred_size = gsk_layer_content_real_get_preferred_size;
  iface->needs_redraw = gsk_layer_content_real_needs_redraw;
  iface->draw = gsk_layer_content_real_draw;
}

gboolean
gsk_layer_content_get_preferred_size (GskLayerContent *content,
                                      graphene_size_t *size)
{
  g_return_val_if_fail (GSK_IS_LAYER_CONTENT (content), FALSE);
  g_return_val_if_fail (size != NULL, FALSE);

  return GSK_LAYER_CONTENT_GET_IFACE (content)->get_preferred_size (content, size);
}

void
gsk_layer_content_draw (GskLayerContent *content,
                        GskLayer        *layer,
                        cairo_t         *cr)
{
  g_return_if_fail (GSK_IS_LAYER_CONTENT (content));
  g_return_if_fail (GSK_IS_LAYER (layer));
  g_return_if_fail (cr != NULL);
  g_return_if_fail (cairo_status (cr) == CAIRO_STATUS_SUCCESS);

  GSK_LAYER_CONTENT_GET_IFACE (content)->draw (content, layer, cr);
}

gboolean
gsk_layer_content_needs_redraw (GskLayerContent *content)
{
  g_return_val_if_fail (GSK_IS_LAYER_CONTENT (content), FALSE);

  return GSK_LAYER_CONTENT_GET_IFACE (content)->needs_redraw (content);
}
