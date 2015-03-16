#include <math.h>
#include <stdlib.h>
#include <gtk/gtk.h>

enum { RED, GREEN, BLUE, N_COLORS };

static GdkRGBA layer_colors[N_COLORS] = {
  { 1.0, 0.0, 0.0, 1.0 },
  { 0.0, 1.0, 0.0, 1.0 },
  { 0.0, 0.0, 1.0, 1.0 },
};

static void
update_scene (GskLayer *root)
{
  GskLayerIter iter;
  GskLayer *child;
  graphene_rect_t bounds;
  float offsets[] = { 0.25, 0.5, 0.75 };
  int i = 0;

  gsk_layer_get_bounds (root, &bounds);

  gsk_layer_iter_init (&iter, root);
  while (gsk_layer_iter_next (&iter, &child))
    {
      graphene_point_t pos;

      graphene_point_init (&pos,
                           bounds.size.width * offsets[i],
                           bounds.size.height * offsets[i]);

      gsk_layer_set_position (child, &pos);

      i += 1;
    }
}

static void
build_scene (GskLayer *root)
{
  graphene_rect_t bounds = GRAPHENE_RECT_INIT (0, 0, 100, 100);
  graphene_rect_t root_bounds;
  float offsets[] = { 0.25, 0.5, 0.75 };
  int i;

  gsk_layer_set_opacity (root, 0.5);

  g_signal_connect (root, "layout-children", G_CALLBACK (update_scene), NULL);

  gsk_layer_get_bounds (root, &root_bounds);

  for (i = 0; i < N_COLORS; i++)
    {
      float x = root_bounds.size.width * offsets[i];
      float y = root_bounds.size.height * offsets[i];
      graphene_euler_t rot;
      graphene_point_t tmp;
      GskLayer *layer;

      graphene_euler_init (&rot, 0, 0, -g_random_double_range (0, 360));

      layer = gsk_layer_new ();
      gsk_layer_set_background_color (layer, &layer_colors[i]);
      gsk_layer_set_opacity (layer, offsets[i]);
      gsk_layer_set_bounds (layer, &bounds);
      gsk_layer_set_position (layer, graphene_point_init (&tmp, x, y));
      gsk_layer_set_scale (layer, offsets[i], offsets[i]);
      gsk_layer_set_rotation (layer, &rot);

      gsk_layer_add_child (root, layer);
    }
}

int
main (int argc, char *argv[])
{
  GtkWidget *window, *box, *area, *button;

  gtk_init (&argc, &argv);

  /* create a new pixel format; we use this to configure the
   * GL context, and to check for features
   */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "GSK");
  gtk_window_set_default_size (GTK_WINDOW (window), 400, 600);
  gtk_container_set_border_width (GTK_CONTAINER (window), 12);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, FALSE);
  gtk_box_set_spacing (GTK_BOX (box), 6);
  gtk_container_add (GTK_CONTAINER (window), box);
  gtk_widget_show (box);

  area = gtk_drawing_area_new ();
  gtk_widget_set_hexpand (area, TRUE);
  gtk_widget_set_vexpand (area, TRUE);
  gtk_widget_set_has_layer (area, TRUE);
  gtk_container_add (GTK_CONTAINER (box), area);
  gtk_widget_show (area);

  build_scene (gtk_widget_get_layer (area));

  button = gtk_button_new_with_mnemonic ("_Quit");
  gtk_widget_set_hexpand (button, TRUE);
  gtk_container_add (GTK_CONTAINER (box), button);
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  gtk_widget_show (button);

  gtk_widget_show (window);

  gtk_main ();

  return EXIT_SUCCESS;
}
