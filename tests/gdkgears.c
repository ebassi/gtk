#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>

#include <epoxy/gl.h>

/* The rendering code in here is taken from glxgears, which has the
 * following copyright notice:
 *
 * Copyright (C) 1999-2001  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

enum {
  X_AXIS,
  Y_AXIS,
  Z_AXIS,

  N_AXIS
};

#define GTK_TYPE_GEARS      (gtk_gears_get_type ())
#define GTK_GEARS(inst)     (G_TYPE_CHECK_INSTANCE_CAST ((inst), \
                             GTK_TYPE_GEARS,             \
                             GtkGears))
#define GTK_IS_GEARS(inst)  (G_TYPE_CHECK_INSTANCE_TYPE ((inst), \
                             GTK_TYPE_GEARS))

typedef struct {
  GtkGLArea parent;

  GLfloat view_rot[N_AXIS];
  GLint gear1, gear2, gear3;
  GLfloat angle;
  gint64 first_frame_time;
  guint tick;
  GtkLabel *fps_label;
} GtkGears;

typedef struct {
  GtkGLAreaClass parent_class;
} GtkGearsClass;

G_DEFINE_TYPE (GtkGears, gtk_gears, GTK_TYPE_GL_AREA);

static gboolean gtk_gears_render        (GtkGLArea     *area,
                                         GdkGLContext  *context);
static void     gtk_gears_size_allocate (GtkWidget     *widget,
                                         GtkAllocation *allocation);
static void     gtk_gears_realize       (GtkWidget     *widget);
static gboolean gtk_gears_tick          (GtkWidget     *widget,
                                         GdkFrameClock *frame_clock,
                                         gpointer       user_data);

GtkWidget *
gtk_gears_new ()
{
  GtkWidget *gears;

  gears = g_object_new (gtk_gears_get_type (),
                        NULL);

  return gears;
}

static void
gtk_gears_init (GtkGears *gears)
{
  gears->view_rot[X_AXIS] = 20.0;
  gears->view_rot[Y_AXIS] = 30.0;
  gears->view_rot[Z_AXIS] = 20.0;

  gears->tick = gtk_widget_add_tick_callback (GTK_WIDGET (gears), gtk_gears_tick, gears, NULL);
}

static void
gtk_gears_finalize (GObject *obj)
{
  GtkGears *gears = GTK_GEARS (obj);

  gtk_widget_remove_tick_callback (GTK_WIDGET (gears), gears->tick);

  g_clear_object (&gears->fps_label);

  G_OBJECT_CLASS (gtk_gears_parent_class)->finalize (obj);
}

static void
gtk_gears_class_init (GtkGearsClass *klass)
{
  GTK_GL_AREA_CLASS (klass)->render = gtk_gears_render;
  GTK_WIDGET_CLASS (klass)->realize = gtk_gears_realize;
  GTK_WIDGET_CLASS (klass)->size_allocate = gtk_gears_size_allocate;
  G_OBJECT_CLASS (klass)->finalize = gtk_gears_finalize;
}

/*
 *
 *  Draw a gear wheel.  You'll probably want to call this function when
 *  building a display list since we do a lot of trig here.
 *
 *  Input:  inner_radius - radius of hole at center
 *          outer_radius - radius at center of teeth
 *          width - width of gear
 *          teeth - number of teeth
 *          tooth_depth - depth of tooth
 */
static void
gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
     GLint teeth, GLfloat tooth_depth)
{
   GLint i;
   GLfloat r0, r1, r2;
   GLfloat angle, da;
   GLfloat u, v, len;

   r0 = inner_radius;
   r1 = outer_radius - tooth_depth / 2.0;
   r2 = outer_radius + tooth_depth / 2.0;

   da = 2.0 * G_PI / teeth / 4.0;

   glShadeModel(GL_FLAT);

   glNormal3f(0.0, 0.0, 1.0);

   /* draw front face */
   glBegin(GL_QUAD_STRIP);
   for (i = 0; i <= teeth; i++) {
      angle = i * 2.0 * M_PI / teeth;
      glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
      glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
      if (i < teeth) {
	 glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
	 glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
		    width * 0.5);
      }
   }
   glEnd();

   /* draw front sides of teeth */
   glBegin(GL_QUADS);
   da = 2.0 * M_PI / teeth / 4.0;
   for (i = 0; i < teeth; i++) {
      angle = i * 2.0 * M_PI / teeth;

      glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
      glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
      glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da),
		 width * 0.5);
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
		 width * 0.5);
   }
   glEnd();

   glNormal3f(0.0, 0.0, -1.0);

   /* draw back face */
   glBegin(GL_QUAD_STRIP);
   for (i = 0; i <= teeth; i++) {
      angle = i * 2.0 * M_PI / teeth;
      glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
      glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
      if (i < teeth) {
	 glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
		    -width * 0.5);
	 glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
      }
   }
   glEnd();

   /* draw back sides of teeth */
   glBegin(GL_QUADS);
   da = 2.0 * M_PI / teeth / 4.0;
   for (i = 0; i < teeth; i++) {
      angle = i * 2.0 * M_PI / teeth;

      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
		 -width * 0.5);
      glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da),
		 -width * 0.5);
      glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
      glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
   }
   glEnd();

   /* draw outward faces of teeth */
   glBegin(GL_QUAD_STRIP);
   for (i = 0; i < teeth; i++) {
      angle = i * 2.0 * M_PI / teeth;

      glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
      glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
      u = r2 * cos(angle + da) - r1 * cos(angle);
      v = r2 * sin(angle + da) - r1 * sin(angle);
      len = sqrt(u * u + v * v);
      u /= len;
      v /= len;
      glNormal3f(v, -u, 0.0);
      glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
      glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
      glNormal3f(cos(angle), sin(angle), 0.0);
      glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da),
		 width * 0.5);
      glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da),
		 -width * 0.5);
      u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
      v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
      glNormal3f(v, -u, 0.0);
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
		 width * 0.5);
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
		 -width * 0.5);
      glNormal3f(cos(angle), sin(angle), 0.0);
   }

   glVertex3f(r1 * cos(0), r1 * sin(0), width * 0.5);
   glVertex3f(r1 * cos(0), r1 * sin(0), -width * 0.5);

   glEnd();

   glShadeModel(GL_SMOOTH);

   /* draw inside radius cylinder */
   glBegin(GL_QUAD_STRIP);
   for (i = 0; i <= teeth; i++) {
      angle = i * 2.0 * M_PI / teeth;
      glNormal3f(-cos(angle), -sin(angle), 0.0);
      glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
      glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
   }
   glEnd();
}

/* new window size or exposure */
static void
reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -40.0);
}

static gboolean
gtk_gears_render (GtkGLArea    *area,
                  GdkGLContext *context)
{
  GtkGears *gears = GTK_GEARS(area);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
  glRotatef(gears->view_rot[X_AXIS], 1.0, 0.0, 0.0);
  glRotatef(gears->view_rot[Y_AXIS], 0.0, 1.0, 0.0);
  glRotatef(gears->view_rot[Z_AXIS], 0.0, 0.0, 1.0);

  glPushMatrix();
  glTranslatef(-3.0, -2.0, 0.0);
  glRotatef(gears->angle, 0.0, 0.0, 1.0);
  glCallList(gears->gear1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(3.1, -2.0, 0.0);
  glRotatef(-2.0 * gears->angle - 9.0, 0.0, 0.0, 1.0);
  glCallList(gears->gear2);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-3.1, 4.2, 0.0);
  glRotatef(-2.0 * gears->angle - 25.0, 0.0, 0.0, 1.0);
  glCallList(gears->gear3);
  glPopMatrix();

  glPopMatrix();

  return TRUE;
}

static void
gtk_gears_size_allocate (GtkWidget     *widget,
                         GtkAllocation *allocation)
{
  GtkGLArea *glarea = GTK_GL_AREA (widget);

  GTK_WIDGET_CLASS (gtk_gears_parent_class)->size_allocate (widget, allocation);

  if (gtk_widget_get_realized (widget))
    {
      if (!gtk_gl_area_make_current (glarea))
        {
          g_warning ("Unable to make gl context current");
          return;
        }

      reshape (allocation->width, allocation->height);
    }
}

static void
gtk_gears_realize (GtkWidget *widget)
{
  GtkGLArea *glarea = GTK_GL_AREA (widget);
  GtkGears *gears = GTK_GEARS(widget);
  GtkAllocation allocation;
  static GLfloat pos[4] = { 5.0, 5.0, 10.0, 0.0 };
  static GLfloat red[4] = { 0.8, 0.1, 0.0, 1.0 };
  static GLfloat green[4] = { 0.0, 0.8, 0.2, 1.0 };
  static GLfloat blue[4] = { 0.2, 0.2, 1.0, 1.0 };

  GTK_WIDGET_CLASS (gtk_gears_parent_class)->realize (widget);

  if (!gtk_gl_area_make_current (glarea))
    {
      g_warning ("Unable to make gl context current");
      return;
    }

  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

  /* make the gears */
  gears->gear1 = glGenLists(1);
  glNewList(gears->gear1, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  gear(1.0, 4.0, 1.0, 20, 0.7);
  glEndList();

  gears->gear2 = glGenLists(1);
  glNewList(gears->gear2, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
  gear(0.5, 2.0, 2.0, 10, 0.7);
  glEndList();

  gears->gear3 = glGenLists(1);
  glNewList(gears->gear3, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
  gear(1.3, 2.0, 0.5, 10, 0.7);
  glEndList();

  glEnable(GL_NORMALIZE);

  gtk_widget_get_allocation (widget, &allocation);
  reshape (allocation.width, allocation.height);
}

static gboolean
gtk_gears_tick (GtkWidget     *widget,
                GdkFrameClock *frame_clock,
                gpointer       user_data)
{
  GtkGears *gears = GTK_GEARS (widget);
  GdkFrameTimings *timings, *previous_timings;
  gint64 previous_frame_time = 0;
  gint64 frame_time;
  gint64 history_start, history_len;
  gint64 frame;
  char *s;

  frame = gdk_frame_clock_get_frame_counter (frame_clock);
  frame_time = gdk_frame_clock_get_frame_time (frame_clock);

  if (gears->first_frame_time == 0)
    {
      /* No need for changes on first frame */
      gears->first_frame_time = frame_time;
      if (gears->fps_label)
        gtk_label_set_label (gears->fps_label, "FPS: ---");
      return G_SOURCE_CONTINUE;
    }

  /* glxgears advances 70 degrees per second, so do the same */

  gears->angle = fmod ((frame_time - gears->first_frame_time) / (double)G_USEC_PER_SEC * 70.0, 360.0);

  gtk_widget_queue_draw (widget);

  history_start = gdk_frame_clock_get_history_start (frame_clock);

  if (gears->fps_label && frame % 60 == 0)
    {
      history_len = frame - history_start;
      if (history_len > 0)
        {
          previous_timings = gdk_frame_clock_get_timings (frame_clock, frame - history_len);
          previous_frame_time = gdk_frame_timings_get_frame_time (previous_timings);

          s = g_strdup_printf ("FPS: %-4.1f", (G_USEC_PER_SEC * history_len) / (double)(frame_time - previous_frame_time));
          gtk_label_set_label (gears->fps_label, s);
          g_free (s);
        }
    }

  timings = gdk_frame_clock_get_current_timings (frame_clock);
  previous_timings = gdk_frame_clock_get_timings (frame_clock,
                                                  gdk_frame_timings_get_frame_counter (timings) - 1);
  if (previous_timings != NULL)
    previous_frame_time = gdk_frame_timings_get_frame_time (previous_timings);

  return G_SOURCE_CONTINUE;
}

void
gtk_gears_set_axis (GtkGears *gears, int axis, double value)
{
  if (axis < 0 || axis >= N_AXIS)
    return;

  gears->view_rot[axis] = value;

  gtk_widget_queue_draw (GTK_WIDGET (gears));
}

double
gtk_gears_get_axis (GtkGears *gears, int axis)
{
  if (axis < 0 || axis >= N_AXIS)
    return 0.0;

  return gears->view_rot[axis];
}

void
gtk_gears_set_fps_label (GtkGears *gears, GtkLabel *label)
{
  if (label)
    g_object_ref (label);

  g_clear_object (&gears->fps_label);

  gears->fps_label = label;
}

/************************************************************************
 *                 DEMO CODE                                            *
 ************************************************************************/

static void
toggle_overlay (GtkWidget *checkbutton,
		GtkWidget *revealer)
{
  gtk_revealer_set_reveal_child (GTK_REVEALER (revealer),
				 gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(checkbutton)));
}

static void
toggle_spin (GtkWidget *checkbutton,
             GtkWidget *spinner)
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(checkbutton)))
    gtk_spinner_start (GTK_SPINNER (spinner));
  else
    gtk_spinner_stop (GTK_SPINNER (spinner));
}

static void
on_axis_value_change (GtkAdjustment *adjustment,
                      gpointer       data)
{
  GtkGears *gears = GTK_GEARS (data);
  int axis = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (adjustment), "axis"));

  gtk_gears_set_axis (gears, axis, gtk_adjustment_get_value (adjustment));
}


static GtkWidget *
create_axis_slider (GtkGears *gears,
                    int axis)
{
  GtkWidget *box, *label, *slider;
  GtkAdjustment *adj;
  const char *text;

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, FALSE);

  switch (axis)
    {
    case X_AXIS:
      text = "X";
      break;

    case Y_AXIS:
      text = "Y";
      break;

    case Z_AXIS:
      text = "Z";
      break;

    default:
      g_assert_not_reached ();
    }

  label = gtk_label_new (text);
  gtk_container_add (GTK_CONTAINER (box), label);
  gtk_widget_show (label);

  adj = gtk_adjustment_new (gtk_gears_get_axis (gears, axis), 0.0, 360.0, 1.0, 12.0, 0.0);
  g_object_set_data (G_OBJECT (adj), "axis", GINT_TO_POINTER (axis));
  g_signal_connect (adj, "value-changed",
                    G_CALLBACK (on_axis_value_change),
                    gears);
  slider = gtk_scale_new (GTK_ORIENTATION_VERTICAL, adj);
  gtk_scale_set_draw_value (GTK_SCALE (slider), FALSE);
  gtk_container_add (GTK_CONTAINER (box), slider);
  gtk_widget_set_vexpand (slider, TRUE);
  gtk_widget_show (slider);

  gtk_widget_show (box);

  return box;
}

static void
moar_gears (GtkButton *button, gpointer data)
{
  GtkContainer *container = GTK_CONTAINER (data);
  GtkWidget *gears;

  gears = gtk_gears_new ();
  gtk_widget_set_size_request (gears, 100, 100);
  gtk_container_add (container, gears);
  gtk_widget_show (gears);
}

int
main (int argc, char *argv[])
{
  GtkWidget *window, *box, *hbox, *button, *spinner, *check,
    *fps_label, *gears, *extra_hbox, *bbox, *overlay,
    *revealer, *frame, *label;
  int i;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "GdkGears");
  gtk_window_set_default_size (GTK_WINDOW (window), 640, 640);
  gtk_container_set_border_width (GTK_CONTAINER (window), 12);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  overlay = gtk_overlay_new ();
  gtk_container_add (GTK_CONTAINER (window), overlay);
  gtk_widget_show (overlay);

  revealer = gtk_revealer_new ();
  gtk_widget_set_halign (revealer, GTK_ALIGN_END);
  gtk_widget_set_valign (revealer, GTK_ALIGN_START);
  gtk_overlay_add_overlay (GTK_OVERLAY (overlay),
			   revealer);
  gtk_widget_show (revealer);

  frame = gtk_frame_new (NULL);
  gtk_style_context_add_class (gtk_widget_get_style_context (frame),
			       "app-notification");
  gtk_container_add (GTK_CONTAINER (revealer), frame);
  gtk_widget_show (frame);

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, FALSE);
  gtk_box_set_spacing (GTK_BOX (hbox), 6);
  gtk_container_add (GTK_CONTAINER (frame), hbox);
  gtk_widget_show (hbox);

  label = gtk_label_new ("This is a transparent overlay widget!!!!");
  gtk_container_add (GTK_CONTAINER (hbox), label);
  gtk_widget_show (label);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, FALSE);
  gtk_box_set_spacing (GTK_BOX (box), 6);
  gtk_container_add (GTK_CONTAINER (overlay), box);
  gtk_widget_show (box);

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, FALSE);
  gtk_box_set_spacing (GTK_BOX (box), 6);
  gtk_container_add (GTK_CONTAINER (box), hbox);
  gtk_widget_show (hbox);

  gears = gtk_gears_new ();
  gtk_widget_set_hexpand (gears, TRUE);
  gtk_widget_set_vexpand (gears, TRUE);
  gtk_container_add (GTK_CONTAINER (hbox), gears);
  gtk_widget_show (gears);

  for (i = 0; i < N_AXIS; i++)
    gtk_container_add (GTK_CONTAINER (hbox), create_axis_slider (GTK_GEARS (gears), i));

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, FALSE);
  gtk_box_set_spacing (GTK_BOX (hbox), 6);
  gtk_container_add (GTK_CONTAINER (box), hbox);
  gtk_widget_show (hbox);

  fps_label = gtk_label_new ("");
  gtk_container_add (GTK_CONTAINER (hbox), fps_label);
  gtk_widget_show (fps_label);
  gtk_gears_set_fps_label (GTK_GEARS (gears), GTK_LABEL (fps_label));

  spinner = gtk_spinner_new ();
  gtk_box_pack_end (GTK_BOX (hbox), spinner, FALSE, FALSE, 0);
  gtk_widget_show (spinner);
  gtk_spinner_start (GTK_SPINNER (spinner));

  check = gtk_check_button_new_with_label ("Animate spinner");
  gtk_box_pack_end (GTK_BOX (hbox), check, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), TRUE);
  gtk_widget_show (check);
  g_signal_connect (check, "toggled",
                    G_CALLBACK (toggle_spin), spinner);

  check = gtk_check_button_new_with_label ("Overlay");
  gtk_box_pack_end (GTK_BOX (hbox), check, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), FALSE);
  gtk_widget_show (check);
  g_signal_connect (check, "toggled",
                    G_CALLBACK (toggle_overlay), revealer);


  extra_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, FALSE);
  gtk_box_set_spacing (GTK_BOX (extra_hbox), 6);
  gtk_container_add (GTK_CONTAINER (box), extra_hbox);
  gtk_widget_show (extra_hbox);

  bbox = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_box_set_spacing (GTK_BOX (bbox), 6);
  gtk_container_add (GTK_CONTAINER (box), bbox);
  gtk_widget_show (bbox);

  button = gtk_button_new_with_label ("Moar gears!");
  gtk_widget_set_hexpand (button, TRUE);
  gtk_container_add (GTK_CONTAINER (bbox), button);
  g_signal_connect (button, "clicked", G_CALLBACK (moar_gears), extra_hbox);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Quit");
  gtk_widget_set_hexpand (button, TRUE);
  gtk_container_add (GTK_CONTAINER (bbox), button);
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  gtk_widget_show (button);

  gtk_widget_show (window);

  gtk_main ();

  return EXIT_SUCCESS;
}
