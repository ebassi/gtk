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

#include "gsklayoutmanager.h"
#include "gsklayerprivate.h"

G_DEFINE_INTERFACE (GskLayoutManager, gsk_layout_manager, G_TYPE_OBJECT)

static void
layout_manager_get_preferred_size (GskLayoutManager *self,
                                   GskLayer         *layer,
                                   graphene_size_t  *size)
{
  const GeometryInfo *info;

  info = gsk_layer_state_get_geometry_info (gsk_layer_get_state (layer));
  graphene_size_init_from_size (size, &info->bounds.size);
}

static void
layout_manager_layout_children (GskLayoutManager *self,
                                GskLayer         *layer)
{
}

static gboolean
layout_manager_queue_relayout (GskLayoutManager *self,
                               GskLayer         *layer,
                               GskLayer         *origin)
{
  return TRUE;
}

static void
gsk_layout_manager_default_init (GskLayoutManagerInterface *iface)
{
  iface->get_preferred_size = layout_manager_get_preferred_size;
  iface->layout_children = layout_manager_layout_children;
  iface->queue_relayout = layout_manager_queue_relayout;
}

void
gsk_layout_manager_get_preferred_size (GskLayoutManager *manager,
                                       GskLayer         *layer,
                                       graphene_size_t  *size)
{
  GskLayoutManagerInterface *iface;

  g_return_if_fail (GSK_IS_LAYOUT_MANAGER (manager));
  g_return_if_fail (GSK_IS_LAYER (layer));
  g_return_if_fail (size != NULL);

  iface = GSK_LAYOUT_MANAGER_GET_IFACE (manager);
  iface->get_preferred_size (manager, layer, size);
}

void
gsk_layout_manager_layout_children (GskLayoutManager *manager,
                                    GskLayer         *layer)
{
  GskLayoutManagerInterface *iface;

  g_return_if_fail (GSK_IS_LAYOUT_MANAGER (manager));
  g_return_if_fail (GSK_IS_LAYER (layer));

  iface = GSK_LAYOUT_MANAGER_GET_IFACE (manager);
  iface->layout_children (manager, layer);
}

gboolean
gsk_layout_manager_queue_relayout (GskLayoutManager *manager,
                                   GskLayer         *layer,
                                   GskLayer         *origin)
{
  GskLayoutManagerInterface *iface;

  g_return_val_if_fail (GSK_IS_LAYOUT_MANAGER (manager), FALSE);
  g_return_val_if_fail (GSK_IS_LAYER (layer), FALSE);
  g_return_val_if_fail (GSK_IS_LAYER (origin), FALSE);

  iface = GSK_LAYOUT_MANAGER_GET_IFACE (manager);
  return iface->queue_relayout (manager, layer, origin);
}
