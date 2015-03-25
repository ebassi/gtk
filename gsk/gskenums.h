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

#ifndef __GSK_ENUMS_H__
#define __GSK_ENUMS_H__

#if !defined (__GSK_H_INSIDE__) && !defined (GSK_COMPILATION)
#error "Only <gsk/gsk.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef enum {
  GSK_LAYOUT_ATTRIBUTE_TOP,
  GSK_LAYOUT_ATTRIBUTE_RIGHT,
  GSK_LAYOUT_ATTRIBUTE_BOTTOM,
  GSK_LAYOUT_ATTRIBUTE_LEFT,

  GSK_LAYOUT_ATTRIBUTE_MIDDLE_X,
  GSK_LAYOUT_ATTRIBUTE_MIDDLE_Y
} GskLayoutAttribute;

G_END_DECLS

#endif /* __GSK_ENUMS_H__ */
