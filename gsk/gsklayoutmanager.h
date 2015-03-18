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

#ifndef __GSK_LAYOUT_MANAGER_H__
#define __GSK_LAYOUT_MANAGER_H__

#if !defined (__GSK_H_INSIDE__) && !defined (GSK_COMPILATION)
#error "Only <gsk/gsk.h> can be included directly."
#endif

#include <gsk/gsktypes.h>

G_BEGIN_DECLS

#define GSK_TYPE_LAYOUT_MANAGER (gsk_layout_manager_get_type ())

GDK_AVAILABLE_IN_3_18
G_DECLARE_INTERFACE (GskLayoutManager, gsk_layout_manager, GSK, LAYOUT_MANAGER, GObject)

struct _GskLayoutManagerInterface
{
  GTypeInterface g_iface;

  void (* get_preferred_size) (GskLayoutManager *manager,
                               GskLayer         *layer,
                               graphene_size_t  *size);

  void (* layout_children) (GskLayoutManager *manager,
                            GskLayer         *layer);

  gboolean (* queue_relayout) (GskLayoutManager *manager,
                               GskLayer         *layer,
                               GskLayer         *origin);
};

GDK_AVAILABLE_IN_3_18
void            gsk_layout_manager_get_preferred_size   (GskLayoutManager *manager,
                                                         GskLayer         *layer,
                                                         graphene_size_t  *size);
GDK_AVAILABLE_IN_3_18
void            gsk_layout_manager_layout_children      (GskLayoutManager *manager,
                                                         GskLayer         *layer);
GDK_AVAILABLE_IN_3_18
gboolean        gsk_layout_manager_queue_relayout       (GskLayoutManager *manager,
                                                         GskLayer         *layer,
                                                         GskLayer         *origin);

G_END_DECLS

#endif /* __GSK_LAYOUT_MANAGER_H__ */
