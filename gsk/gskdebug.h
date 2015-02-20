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

#ifndef __GSK_DEBUG_H__
#define __GSK_DEBUG_H__

#if !defined (__GSK_H_INSIDE__) && !defined (GSK_COMPILATION)
#error "Only <gsk/gsk.h> can be included directly."
#endif

#include <gsk/gsktypes.h>

G_BEGIN_DECLS

typedef enum {
  GSK_DEBUG_GEOMETRY = 1 << 0,
  GSK_DEBUG_RENDER = 1 << 1,
  GSK_DEBUG_LAYOUT = 1 << 2
} GskDebugFlags;

#ifdef G_ENABLE_DEBUG
# define GSK_NOTE(type,action)          G_STMT_START {  \
  if (gsk_get_debug_flags () & GSK_DEBUG_ ## type) {    \
    action;                                             \
  }                                     } G_STMT_END

#else
# define GSK_NOTE(type,action)
#endif

GDK_AVAILABLE_IN_3_18
guint gsk_get_debug_flags (void);

GDK_AVAILABLE_IN_3_18
void  gsk_set_debug_flags (guint flags);

G_END_DECLS

#endif /* __GSK_DEBUG_H__ */
