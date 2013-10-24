#include "config.h"

#include <math.h>

#include "cluttertypes.h"

#define FLOAT_EPSILON   (1e-15)

static ClutterPoint *   clutter_point_copy      (ClutterPoint *point);

static const ClutterPoint _clutter_point_zero = CLUTTER_POINT_INIT_ZERO;

G_DEFINE_BOXED_TYPE (ClutterPoint, clutter_point, clutter_point_copy, clutter_point_free)

/**
 * clutter_point_zero:
 *
 * A point centered at (0, 0).
 *
 * The returned value can be used as a guard.
 *
 * Returns: (transfer none): a point centered in (0, 0); the returned
 *   #ClutterPoint is owned by Clutter and it should not be modified
 *   or freed
 *
 * Since: 3.12
 */
const ClutterPoint *
clutter_point_zero (void)
{
  return &_clutter_point_zero;
}

/**
 * clutter_point_alloc: (constructor)
 *
 * Allocates a new #ClutterPoint.
 *
 * Returns: (transfer full): the newly allocated #ClutterPoint.
 *   Use clutter_point_free() to free its resources.
 *
 * Since: 3.12
 */
ClutterPoint *
clutter_point_alloc (void)
{
  return g_slice_new0 (ClutterPoint);
}

/**
 * clutter_point_init:
 * @point: a #ClutterPoint
 * @x: the X coordinate of the point
 * @y: the Y coordinate of the point
 *
 * Initializes @point with the given coordinates.
 *
 * Returns: (transfer none): the initialized #ClutterPoint
 *
 * Since: 3.12
 */
ClutterPoint *
clutter_point_init (ClutterPoint *point,
                    float         x,
                    float         y)
{
  g_return_val_if_fail (point != NULL, NULL);

  point->x = x;
  point->y = y;

  return point;
}

/**
 * clutter_point_init_with_point:
 * @point: a #ClutterPoint
 * @src: a #ClutterPoint
 *
 * Initializes @point with the coordinates of the given @src #ClutterPoint.
 *
 * Returns: (transfer none): the initialized #ClutterPoint
 *
 * Since: 3.12
 */
ClutterPoint *
clutter_point_init_with_point (ClutterPoint       *point,
                               const ClutterPoint *src)
{
  g_return_val_if_fail (point != NULL, NULL);
  g_return_val_if_fail (src != NULL, point);

  *point = *src;

  return point;
}

static ClutterPoint *
clutter_point_copy (ClutterPoint *point)
{
  if (G_LIKELY (point))
    return g_slice_dup (ClutterPoint, point);

  return NULL;
}

/**
 * clutter_point_free:
 * @point: a #ClutterPoint
 *
 * Frees the resources allocated for @point.
 *
 * Since: 3.12
 */
void
clutter_point_free (ClutterPoint *point)
{
  if (G_LIKELY (point != NULL && point != &_clutter_point_zero))
    g_slice_free (ClutterPoint, point);
}

/**
 * clutter_point_equal:
 * @v1: the first #ClutterPoint to compare
 * @v2: the second #ClutterPoint to compare
 *
 * Compares two #ClutterPoint for equality.
 *
 * Returns: %TRUE if the #ClutterPoints are equal
 *
 * Since: 3.12
 */
gboolean
clutter_point_equal (gconstpointer v1,
                     gconstpointer v2)
{
  const ClutterPoint *a = v1;
  const ClutterPoint *b = v2;

  if (a == b)
    return TRUE;

  if (a == NULL || b == NULL)
    return FALSE;

  return fabsf (a->x - b->x) < FLOAT_EPSILON &&
         fabsf (a->y - b->y) < FLOAT_EPSILON;
}

/**
 * clutter_point_distance:
 * @p1: a #ClutterPoint
 * @p2: a #ClutterPoint
 * @delta_x: (out) (allow-none): return location for the horizontal
 *   distance between the points
 * @delta_y: (out) (allow-none): return location for the vertical
 *   distance between the points
 *
 * Computes the distance between two #ClutterPoint<!-- -->s.
 *
 * Returns: the distance between the points.
 *
 * Since: 3.12
 */
float
clutter_point_distance (const ClutterPoint *p1,
                        const ClutterPoint *p2,
                        float              *delta_x,
                        float              *delta_y)
{
  float x_d, y_d;

  g_return_val_if_fail (p1 != NULL, 0.f);
  g_return_val_if_fail (p2 != NULL, 0.f);

  if (clutter_point_equal (p1, p2))
    {
      if (delta_x != NULL)
        *delta_x = 0.f;

      if (delta_y != NULL)
        *delta_y = 0.f;

      return 0.f;
    }

  x_d = (p1->x - p2->x);
  y_d = (p1->y - p2->y);

  if (delta_x != NULL)
    *delta_x = fabsf (x_d);

  if (delta_y != NULL)
    *delta_y = fabsf (y_d);

  return sqrt ((x_d * x_d) + (y_d * y_d));
}

/**
 * clutter_point_interpolate:
 * @p1: a #ClutterPoint
 * @p2: a #ClutterPoint
 * @factor: the interpolation factor
 * @res: (out) (caller allocates): return location for the interpolated point
 *
 * Linearly interpolates the horizontal and vertical components of @p1
 * and @p2 using the given @factor, and places the results inside @res.
 *
 * Since: 3.12
 */
void
clutter_point_interpolate (const ClutterPoint *p1,
                           const ClutterPoint *p2,
                           double              factor,
                           ClutterPoint       *res)
{
  g_return_if_fail (p1 != NULL);
  g_return_if_fail (p2 != NULL);
  g_return_if_fail (res != NULL);

  res->x = p1->x + (p2->x - p1->x) * factor;
  res->y = p1->y + (p2->y - p1->y) * factor;
}
