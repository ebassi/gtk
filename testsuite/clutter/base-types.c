#include <clutter/clutter.h>
#include <math.h>

static void
test_types_point_init (void)
{
  ClutterPoint point;

  clutter_point_init (&point, 0.f, 0.f);

  g_assert_true (clutter_point_equal (&point, clutter_point_zero ()));

  clutter_point_init_with_point (&point, clutter_point_zero ());
  g_assert_true (clutter_point_equal (&point, clutter_point_zero ()));

  clutter_point_init (&point, 100.f, 15.f);
  g_assert_cmpfloat (point.x, ==, 100.f);
  g_assert_cmpfloat (point.y, ==, 15.f);
}

static void
test_types_point_distance (void)
{
  ClutterPoint p1 = CLUTTER_POINT_INIT (0.f, 0.f);
  ClutterPoint p2 = CLUTTER_POINT_INIT (1.f, 1.f);
  float d_x, d_y;

  g_assert_false (clutter_point_equal (&p1, &p2));
  g_assert_cmpfloat (clutter_point_distance (&p1, &p2, &d_x, &d_y), ==, sqrtf (2.f));
  g_assert_cmpfloat (d_x, ==, 1.f);
  g_assert_cmpfloat (d_y, ==, 1.f);

  clutter_point_init_with_point (&p2, &p1);
  g_assert_true (clutter_point_equal (&p1, &p2));
  g_assert_cmpfloat (clutter_point_distance (&p1, &p2, &d_x, &d_y), ==, 0.f);
  g_assert_cmpfloat (d_x, ==, 0.f);
  g_assert_cmpfloat (d_y, ==, 0.f);

  clutter_point_init (&p2, 3.f, 4.f);
  g_assert_cmpfloat (clutter_point_distance (&p1, &p2, NULL, NULL), ==, 5.f);
}

static void
test_types_point_interpolate (void)
{
  ClutterPoint p1 = CLUTTER_POINT_INIT (0.f, 0.f);
  ClutterPoint p2 = CLUTTER_POINT_INIT (1.f, 1.f);
  ClutterPoint res;

  clutter_point_interpolate (&p1, &p2, 0.0, &res);
  g_assert_cmpfloat (res.x, ==, 0.f);
  g_assert_cmpfloat (res.y, ==, 0.f);
  g_assert_true (clutter_point_equal (&p1, &res));

  clutter_point_interpolate (&p1, &p2, 1.0, &res);
  g_assert_cmpfloat (res.x, ==, 1.f);
  g_assert_cmpfloat (res.y, ==, 1.f);
  g_assert_true (clutter_point_equal (&p2, &res));

  clutter_point_interpolate (&p1, &p2, 0.5, &res);
  g_assert_false (clutter_point_equal (&p1, &res));
  g_assert_false (clutter_point_equal (&p2, &res));
  g_assert_cmpfloat (res.x, ==, 0.5f);
  g_assert_cmpfloat (res.y, ==, 0.5f);
}

static void
test_types_size_init (void)
{
  ClutterSize check = CLUTTER_SIZE_INIT (100.f, 50.f);
  ClutterSize size;

  clutter_size_init (&size, 100.f, 50.f);
  g_assert_cmpfloat (size.height, ==, 50.f);
  g_assert_cmpfloat (size.width, ==, 100.f);

  clutter_size_init_with_size (&size, &check);
  g_assert_true (clutter_size_equal (&size, &check));

  clutter_size_init (&size, 50.f, 100.f);
  g_assert_false (clutter_size_equal (&size, &check));
}

static void
test_types_size_interpolate (void)
{
  ClutterSize s1 = CLUTTER_SIZE_INIT (100.f, 50.f);
  ClutterSize s2 = CLUTTER_SIZE_INIT (50.f, 100.f);
  ClutterSize res;

  g_assert_false (clutter_size_equal (&s1, &s2));

  clutter_size_interpolate (&s1, &s2, 0.0, &res);
  g_assert_true (clutter_size_equal (&s1, &res));

  clutter_size_interpolate (&s1, &s2, 1.0, &res);
  g_assert_true (clutter_size_equal (&s2, &res));

  clutter_size_interpolate (&s1, &s2, 0.5, &res);
  g_assert_cmpfloat (res.width, ==, s1.width + (s2.width - s1.width) * 0.5);
  g_assert_cmpfloat (res.height, ==, s1.height + (s2.height - s1.height) * 0.5);
}

static void
test_types_rect_init (void)
{
  ClutterRect check;
  ClutterRect rect;

  clutter_rect_init (&rect, 0.f, 0.f, 100.f, 100.f);
  g_assert_cmpfloat (clutter_rect_get_x (&rect), ==, 0.f);
  g_assert_cmpfloat (clutter_rect_get_y (&rect), ==, 0.f);
  g_assert_cmpfloat (clutter_rect_get_width (&rect), ==, 100.f);
  g_assert_cmpfloat (clutter_rect_get_height (&rect), ==, 100.f);

  clutter_rect_init_with_rect (&rect, clutter_rect_zero ());
  clutter_rect_init_with_rect (&check, clutter_rect_zero ());
  g_assert_true (clutter_rect_equal (&rect, &check));
}

static void
test_types_rect_normalize (void)
{
  ClutterRect check = CLUTTER_RECT_INIT (10.f, 15.f, 50.f, 100.f);
  ClutterRect rect;

  clutter_rect_init (&rect, 10.f, 15.f, 50.f, 100.f);
  g_assert_true (clutter_rect_equal (&rect, &check));

  clutter_rect_init (&rect, 60.f, 115.f, -50.f, -100.f);
  g_assert_true (clutter_rect_equal (&rect, &check));
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/types/point/init", test_types_point_init);
  g_test_add_func ("/types/point/distance", test_types_point_distance);
  g_test_add_func ("/types/point/interpolate", test_types_point_interpolate);

  g_test_add_func ("/types/size/init", test_types_size_init);
  g_test_add_func ("/types/size/interpolate", test_types_size_interpolate);

  g_test_add_func ("/types/rect/init", test_types_rect_init);
  g_test_add_func ("/types/rect/normalize", test_types_rect_normalize);

  return g_test_run ();
}
