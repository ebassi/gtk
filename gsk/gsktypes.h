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

#ifndef __GSK_TYPES_H__
#define __GSK_TYPES_H__

#if !defined (__GSK_H_INSIDE__) && !defined (GSK_COMPILATION)
#error "Only <gsk/gsk.h> can be included directly."
#endif

#include <glib.h>
#include <graphene.h>
#include <gdk/gdk.h>
#include <gsk/gskmacros.h>
#include <gsk/gskenums.h>

G_BEGIN_DECLS

typedef struct _GskLayer                GskLayer;
typedef struct _GskLayerIter            GskLayerIter;
typedef struct _GskRenderer             GskRenderer;
typedef struct _GskLayoutManager        GskLayoutManager;

G_END_DECLS

#endif /* __GSK_TYPES_H__ */
