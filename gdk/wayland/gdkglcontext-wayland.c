/* GDK - The GIMP Drawing Kit
 *
 * gdkglcontext-wayland.c: Wayland specific OpenGL wrappers
 *
 * Copyright © 2014  Emmanuele Bassi
 * Copyright © 2014  Alexander Larsson
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "gdkglcontext-wayland.h"
#include "gdkdisplay-wayland.h"

#include "gdkwaylanddisplay.h"
#include "gdkwaylandglcontext.h"
#include "gdkwaylandwindow.h"

#include "gdkinternals.h"

#include "gdkintl.h"

G_DEFINE_TYPE (GdkWaylandGLContext, gdk_wayland_gl_context, GDK_TYPE_GL_CONTEXT)

static void
gdk_wayland_gl_context_set_window (GdkGLContext *context,
                                   GdkWindow    *window)
{
  //GdkWaylandGLContext *context_wayland = GDK_WAYLAND_GL_CONTEXT (context);
  GdkDisplay *display = gdk_gl_context_get_display (context);

  if (window == NULL)
    {
      gdk_wayland_display_make_gl_context_current (display, context, NULL);
      return;
    }

}

static void
gdk_wayland_gl_context_update (GdkGLContext *context,
                               GdkWindow    *window)
{
  GdkDisplay *display = gdk_gl_context_get_display (context);
  int width, height;

  if (!gdk_wayland_display_make_gl_context_current (display, context, window))
    return;

  width = gdk_window_get_width (window);
  height = gdk_window_get_height (window);

  GDK_NOTE (OPENGL, g_print ("Updating GL viewport size to { %d, %d } for window %p (context: %p)\n",
                             width, height,
                             window, context));

  glViewport (0, 0, width, height);
}

static void
gdk_wayland_gl_context_flush_buffer (GdkGLContext *context)
{
  GdkDisplay *display = gdk_gl_context_get_display (context);
  GdkWindow *window = gdk_gl_context_get_window (context);

  if (window == NULL)
    return;

  gdk_wayland_display_make_gl_context_current (display, context, window);

  /* TODO:  */
}

static void
gdk_wayland_gl_context_class_init (GdkWaylandGLContextClass *klass)
{
  GdkGLContextClass *context_class = GDK_GL_CONTEXT_CLASS (klass);

  context_class->set_window = gdk_wayland_gl_context_set_window;
  context_class->update = gdk_wayland_gl_context_update;
  context_class->flush_buffer = gdk_wayland_gl_context_flush_buffer;
}

static void
gdk_wayland_gl_context_init (GdkWaylandGLContext *self)
{
}

gboolean
gdk_wayland_display_init_gl (GdkDisplay *display)
{
  GdkWaylandDisplay *display_wayland = GDK_WAYLAND_DISPLAY (display);
  EGLint major, minor;
  EGLDisplay *dpy;

  if (display_wayland->have_egl)
    return TRUE;

  dpy = eglGetDisplay ((EGLNativeDisplayType)display_wayland->wl_display);
  if (dpy == NULL)
    return FALSE;

  if (!eglInitialize (dpy, &major, &minor))
    return FALSE;

  if (!eglBindAPI (EGL_OPENGL_API))
    return FALSE;

  display_wayland->egl_display = dpy;
  display_wayland->egl_major_version = major;
  display_wayland->egl_minor_version = minor;

  display_wayland->have_egl = TRUE;

  display_wayland->have_egl_khr_create_context =
    epoxy_has_egl_extension (dpy, "EGL_KHR_create_context");

  GDK_NOTE (OPENGL,
            g_print ("EGL API version %d.%d found\n"
                     " - Vendor: %s\n"
                     " - Version: %s\n"
                     " - Client APIs: %s\n"
                     " - Extensions:\n"
                     "\t%s\n"
                     ,
                     display_wayland->egl_major_version,
                     display_wayland->egl_minor_version,
                     eglQueryString (dpy, EGL_VENDOR),
                     eglQueryString(dpy, EGL_VERSION),
                     eglQueryString(dpy, EGL_CLIENT_APIS),
                     eglQueryString(dpy, EGL_EXTENSIONS)));

  return TRUE;
}

#define MAX_EGL_ATTRS   30

static void
get_egl_attributes_for_pixel_format (GdkDisplay       *display,
                                     GdkGLPixelFormat *format,
                                     EGLint              *attrs)
{
  int i = 0;

  attrs[i++] = EGL_SURFACE_TYPE;
  attrs[i++] = EGL_WINDOW_BIT;

  attrs[i++] = EGL_COLOR_BUFFER_TYPE;
  attrs[i++] = EGL_RGB_BUFFER;

  if (format->double_buffer)
    {
      /* TODO: I don't think this makes sense for egl, does it? */
    }

  if (format->stereo)
    {
      /* TODO: I don't think this makes sense for egl, does it? */
    }

  if (format->color_size < 0)
    {
      attrs[i++] = EGL_RED_SIZE;
      attrs[i++] = 1;
      attrs[i++] = EGL_GREEN_SIZE;
      attrs[i++] = 1;
      attrs[i++] = EGL_BLUE_SIZE;
      attrs[i++] = 1;
    }
  else
    {
      attrs[i++] = EGL_RED_SIZE;
      attrs[i++] = format->color_size;
      attrs[i++] = EGL_GREEN_SIZE;
      attrs[i++] = format->color_size;
      attrs[i++] = EGL_BLUE_SIZE;
      attrs[i++] = format->color_size;
    }

  if (format->alpha_size < 0)
    {
      attrs[i++] = EGL_ALPHA_SIZE;
      attrs[i++] = 1;
    }
  else if (format->alpha_size == 0)
    {
      attrs[i++] = EGL_ALPHA_SIZE;
      attrs[i++] = 0;
    }
  else
    {
      attrs[i++] = EGL_ALPHA_SIZE;
      attrs[i++] = format->alpha_size;
    }

  if (format->depth_size < 0)
    {
      attrs[i++] = EGL_DEPTH_SIZE;
      attrs[i++] = 1;
    }
  else
    {
      attrs[i++] = EGL_DEPTH_SIZE;
      attrs[i++] = format->depth_size;
    }

  if (format->stencil_size < 0)
    {
      attrs[i++] = EGL_STENCIL_SIZE;
      attrs[i++] = 0;
    }
  else
    {
      attrs[i++] = EGL_STENCIL_SIZE;
      attrs[i++] = format->stencil_size;
    }

  if (format->accum_size > 0)
    {
      /* TODO: I don't think this makes sense for egl, does it? */
    }

  if (format->multi_sample)
    {
      attrs[i++] = EGL_SAMPLE_BUFFERS;
      attrs[i++] = format->sample_buffers > 0 ? format->sample_buffers : 1;

      attrs[i++] = EGL_SAMPLES;
      attrs[i++] = format->samples > 0 ? format->samples : 1;
    }

  attrs[i++] = EGL_NONE;

  g_assert (i < MAX_EGL_ATTRS);
}

static gboolean
find_eglconfig_for_pixel_format (GdkDisplay        *display,
                                 GdkGLPixelFormat  *format,
                                 EGLConfig         *egl_config_out,
                                 GError           **error)
{
  GdkWaylandDisplay *display_wayland = GDK_WAYLAND_DISPLAY (display);
  EGLint attrs[MAX_EGL_ATTRS];
  EGLint count;
  EGLConfig *configs;

  get_egl_attributes_for_pixel_format (display, format, attrs);

  if (!eglChooseConfig (display_wayland->egl_display, attrs, NULL, 0, &count) || count < 1)
    {
      g_set_error_literal (error, GDK_GL_PIXEL_FORMAT_ERROR,
                           GDK_GL_PIXEL_FORMAT_ERROR_NOT_AVAILABLE,
                           _("No available configurations for the given pixel format"));
      return FALSE;
    }

  configs = g_new (EGLConfig, count);

  if (!eglChooseConfig (display_wayland->egl_display, attrs, configs, count, &count) || count < 1)
    {
      g_set_error_literal (error, GDK_GL_PIXEL_FORMAT_ERROR,
                           GDK_GL_PIXEL_FORMAT_ERROR_NOT_AVAILABLE,
                           _("No available configurations for the given pixel format"));
      return FALSE;
    }

  /* Pick first valid configuration i guess? */

  if (egl_config_out != NULL)
    *egl_config_out = configs[0];

  g_free (configs);

  return TRUE;
}

static void
update_pixel_format (GdkDisplay       *display,
                     GdkGLPixelFormat *format,
                     EGLConfig       config)
{
  GdkWaylandDisplay *display_wayland = GDK_WAYLAND_DISPLAY (display);
  EGLDisplay dpy = display_wayland->egl_display;
  int value = 0;

  /* format->double_buffer => makes no sense? */

  eglGetConfigAttrib (dpy, config, EGL_RED_SIZE, &value);
  format->color_size = value;
  eglGetConfigAttrib (dpy, config, EGL_GREEN_SIZE, &value);
  format->color_size = MIN (format->color_size, value);
  eglGetConfigAttrib (dpy, config, EGL_BLUE_SIZE, &value);
  format->color_size = MIN (format->color_size, value);

  eglGetConfigAttrib (dpy, config, EGL_ALPHA_SIZE, &format->alpha_size);

  /* aux buffers - make no sense?  */

  eglGetConfigAttrib (dpy, config, EGL_DEPTH_SIZE, &format->depth_size);
  eglGetConfigAttrib (dpy, config, EGL_STENCIL_SIZE, &format->stencil_size);

  /* accum_size make no sense */
}

GdkGLContext *
gdk_wayland_display_create_gl_context (GdkDisplay        *display,
                                       GdkGLPixelFormat  *format,
                                       GdkGLContext      *share,
                                       GError           **error)
{
  GdkWaylandDisplay *display_wayland = GDK_WAYLAND_DISPLAY (display);
  GdkWaylandGLContext *context;
  GdkGLPixelFormat *valid_format;
  EGLContext ctx;
  EGLConfig config;
  int i;
  EGLint context_attribs[3];

  if (!gdk_wayland_display_validate_gl_pixel_format (display, format, NULL, error))
    return NULL;

  /* if validation succeeded, then we don't need to check for the
   * result here: we know the pixel format has a valid EGLConfig
   */
  find_eglconfig_for_pixel_format (display, format, &config, NULL);

  i = 0;
  if (format->profile == GDK_GL_PIXEL_FORMAT_PROFILE_3_2_CORE)
    {
      context_attribs[i++] = EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR;
      context_attribs[i++] = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR;
    }
  context_attribs[i++] = EGL_NONE;

  ctx = eglCreateContext (display_wayland->egl_display,
                          config,
                          share ? GDK_WAYLAND_GL_CONTEXT (share)->egl_context : EGL_NO_CONTEXT,
                          context_attribs);
  if (ctx == NULL)
    {
      g_set_error_literal (error, GDK_GL_PIXEL_FORMAT_ERROR,
                           GDK_GL_PIXEL_FORMAT_ERROR_NOT_AVAILABLE,
                           _("Unable to create a GL context"));
      return NULL;
    }


  GDK_NOTE (OPENGL,
            g_print ("Created EGL context[%p]\n", ctx));

    /* the GdkGLContext holds a reference on the pixel format
   * that is used to create it, not the one that the user
   * passed; this allows the user to query the pixel format
   * attributes
   */
  valid_format = g_object_new (GDK_TYPE_GL_PIXEL_FORMAT, NULL);
  update_pixel_format (display, valid_format, config);

  context = g_object_new (GDK_WAYLAND_TYPE_GL_CONTEXT,
                          "display", display,
                          "pixel-format", valid_format,
                          "visual", gdk_screen_get_system_visual (gdk_display_get_default_screen (display)),
                          NULL);

  context->egl_config = config;
  context->egl_context = ctx;

  g_object_unref (valid_format);

  return GDK_GL_CONTEXT (context);
}

void
gdk_wayland_display_destroy_gl_context (GdkDisplay   *display,
                                        GdkGLContext *context)
{
  GdkWaylandDisplay *display_wayland = GDK_WAYLAND_DISPLAY (display);
  GdkWaylandGLContext *context_wayland = GDK_WAYLAND_GL_CONTEXT (context);

  /* TODO: Unset as current if current? */

  eglDestroyContext (display_wayland->egl_display,
                     context_wayland->egl_context);

  context_wayland->egl_context = NULL;
}

gboolean
gdk_wayland_display_make_gl_context_current (GdkDisplay   *display,
                                             GdkGLContext *context,
                                             GdkWindow    *window)
{
  GdkWaylandDisplay *display_wayland = GDK_WAYLAND_DISPLAY (display);
  GdkWaylandGLContext *context_wayland = GDK_WAYLAND_GL_CONTEXT (context);
  EGLSurface egl_surface;

  /* TODO: Passing NULL for config here will break if the egl_surface was not set before */
  egl_surface = gdk_wayland_window_get_egl_surface (window->impl_window, NULL);

  if (!eglMakeCurrent(display_wayland->egl_display, egl_surface,
                      egl_surface, context_wayland->egl_context))
    return FALSE;

  return TRUE;
}

gboolean
gdk_wayland_display_validate_gl_pixel_format (GdkDisplay        *display,
                                              GdkGLPixelFormat  *format,
                                              GdkGLPixelFormat **validated_format,
                                              GError           **error)
{
  GdkWaylandDisplay *display_wayland = GDK_WAYLAND_DISPLAY (display);
  EGLConfig config;

  if (!gdk_wayland_display_init_gl (display))
    {
      g_set_error_literal (error, GDK_GL_PIXEL_FORMAT_ERROR,
                           GDK_GL_PIXEL_FORMAT_ERROR_NOT_AVAILABLE,
                           _("No GL implementation is available"));
      return FALSE;
    }

  if (format->profile == GDK_GL_PIXEL_FORMAT_PROFILE_3_2_CORE &&
      !display_wayland->have_egl_khr_create_context)
    {
      g_set_error_literal (error, GDK_GL_PIXEL_FORMAT_ERROR,
                           GDK_GL_PIXEL_FORMAT_ERROR_NOT_AVAILABLE,
                           _("3.2 core GL profile is not available on EGL implementation"));
      return FALSE;
    }

  if (!find_eglconfig_for_pixel_format (display, format, &config, error))
    {
      return FALSE;
    }

  GDK_NOTE (OPENGL,
            g_print ("Found EGL config for requested pixel format:\n"
                     " - double-buffer: %s\n"
                     " - multi-sample: %s\n"
                     " - stereo: %s\n"
                     " - color-size: %d, alpha-size: %d\n"
                     " - depth-size: %d\n"
                     " - stencil-size: %d\n"
                     " - aux-buffers: %d\n"
                     " - accum-size: %d\n"
                     " - sample-buffers: %d, samples: %d\n",
                     format->double_buffer ? "yes" : "no",
                     format->multi_sample ? "yes" : "no",
                     format->stereo ? "yes" : "no",
                     format->color_size, format->alpha_size,
                     format->depth_size,
                     format->stencil_size,
                     format->aux_buffers,
                     format->accum_size,
                     format->sample_buffers, format->samples));

  if (validated_format != NULL)
    {
      GdkGLPixelFormat *valid = g_object_new (GDK_TYPE_GL_PIXEL_FORMAT, NULL);

      /* update the pixel format with the values of the
       * configuration we found
       */
      update_pixel_format (display, valid, config);

      *validated_format = valid;
    }

  return TRUE;
}
