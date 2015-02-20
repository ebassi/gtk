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

#include "config.h"

#include "gskdebug.h"

static guint gsk_debug_flags;

#ifdef G_ENABLE_DEBUG
static const GDebugKey gsk_debug_keys[] = {
  { "geometry", GSK_DEBUG_GEOMETRY },
  { "render", GSK_DEBUG_RENDER },
  { "layout", GSK_DEBUG_LAYOUT },
};
#endif /* G_ENABLE_DEBUG */

guint
gsk_get_debug_flags (void)
{
#ifdef G_ENABLE_DEBUG
  static gboolean env_check;

  if (G_UNLIKELY (!env_check))
    {
      const char *tmp = g_getenv ("GSK_DEBUG");

      env_check = TRUE;
      if (tmp != NULL)
        gsk_debug_flags = g_parse_debug_string (tmp, gsk_debug_keys,
                                                G_N_ELEMENTS (gsk_debug_keys));
    }
#endif

  return gsk_debug_flags;
}

void
gsk_set_debug_flags (guint flags)
{
  gsk_debug_flags = flags;
}
