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

#ifndef __GSK_LAYOUT_CONSTRAINT_H__
#define __GSK_LAYOUT_CONSTRAINT_H__

#if !defined (__GSK_H_INSIDE__) && !defined (GSK_COMPILATION)
#error "Only <gsk/gsk.h> can be included directly."
#endif

#include <gsk/gsktypes.h>

G_BEGIN_DECLS

#define GSK_TYPE_LAYOUT_CONSTRAINT (gsk_layout_constraint_get_type ())

GDK_AVAILABLE_IN_3_18
G_DECLARE_FINAL_TYPE (GskLayoutConstraint, gsk_layout_constraint, GSK, LAYOUT_CONSTRAINT, GObject)

GDK_AVAILABLE_IN_3_18
GskLayoutConstraint *   gsk_layout_constraint_new               (gpointer             source,
                                                                 GskLayoutAttribute   source_attr,
                                                                 gpointer             target,
                                                                 GskLayoutAttribute   target_attr,
                                                                 double               constant,
                                                                 double               factor);

GDK_AVAILABLE_IN_3_18
void                    gsk_layout_constraint_set_constant      (GskLayoutConstraint *self,
                                                                 double               value);
GDK_AVAILABLE_IN_3_18
void                    gsk_layout_constraint_set_factor        (GskLayoutConstraint *self,
                                                                 double               value);

G_END_DECLS

#endif /* __GSK_LAYOUT_CONSTRAINT_H__ */
