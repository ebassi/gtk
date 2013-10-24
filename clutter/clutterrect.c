#include "config.h"

#include <math.h>

#include "cluttertypes.h"

static ClutterRect *    clutter_rect_copy       (ClutterRect *rect);

static const ClutterRect _clutter_rect_zero = CLUTTER_RECT_INIT_ZERO;

G_DEFINE_BOXED_TYPE (ClutterRect, clutter_rect, clutter_rect_copy, clutter_rect_free)

static inline void
clutter_rect_normalize_internal (ClutterRect *rect)
{
  if (rect->size.width >= 0.f && rect->size.height >= 0.f)
    return;

  if (rect->size.width < 0.f)
    {
      float size = fabsf (rect->size.width);

      rect->origin.x -= size;
      rect->size.width = size;
    }

  if (rect->size.height < 0.f)
    {
      float size = fabsf (rect->size.height);

      rect->origin.y -= size;
      rect->size.height = size;
    }
}

/**
 * clutter_rect_zero:
 *
 * A #ClutterRect with #ClutterRect.origin set at (0, 0) and a size
 * of 0.
 *
 * The returned value can be used as a guard.
 *
 * Returns: a rectangle with origin in (0, 0) and a size of 0.
 *   The returned #ClutterRect is owned by Clutter and it should not
 *   be modified or freed.
 *
 * Since: 3.12
 */
const ClutterRect *
clutter_rect_zero (void)
{
  return &_clutter_rect_zero;
}

/**
 * clutter_rect_alloc: (constructor)
 *
 * Creates a new, empty #ClutterRect.
 *
 * You can use clutter_rect_init() to initialize the returned rectangle,
 * for instance:
 *
 * |[
 *   rect = clutter_rect_init (clutter_rect_alloc (), x, y, width, height);
 * ]|
 *
 * Returns: (transfer full): the newly allocated #ClutterRect.
 *   Use clutter_rect_free() to free its resources
 *
 * Since: 3.12
 */
ClutterRect *
clutter_rect_alloc (void)
{
  return g_slice_new0 (ClutterRect);
}

/**
 * clutter_rect_init:
 * @rect: a #ClutterRect
 * @x: X coordinate of the origin
 * @y: Y coordinate of the origin
 * @width: width of the rectangle
 * @height: height of the rectangle
 *
 * Initializes a #ClutterRect with the given origin and size.
 *
 * Returns: (transfer none): the updated rectangle
 *
 * Since: 3.12
 */
ClutterRect *
clutter_rect_init (ClutterRect *rect,
                   float        x,
                   float        y,
                   float        width,
                   float        height)
{
  g_return_val_if_fail (rect != NULL, NULL);

  rect->origin.x = x;
  rect->origin.y = y;

  rect->size.width = width;
  rect->size.height = height;

  clutter_rect_normalize_internal (rect);

  return rect;
}

/**
 * clutter_rect_init_with_rect:
 * @rect: a #ClutterRect
 * @src: a #ClutterRect, used to initiale @rect
 *
 * Initializes @rect using the origin and size of @src.
 *
 * Returns: (transfer none): the initialized #ClutterRect
 *
 * Since: 3.12
 */
ClutterRect *
clutter_rect_init_with_rect (ClutterRect       *rect,
                             const ClutterRect *src)
{
  g_return_val_if_fail (rect != NULL, NULL);
  g_return_val_if_fail (src != NULL, rect);

  *rect = *src;

  clutter_rect_normalize_internal (rect);

  return rect;
}

static ClutterRect *
clutter_rect_copy (ClutterRect *rect)
{
  if (rect != NULL)
    {
      ClutterRect *res;

      res = g_slice_dup (ClutterRect, rect);
      clutter_rect_normalize_internal (res);

      return res;
    }

  return NULL;
}

/**
 * clutter_rect_free:
 * @rect: a #ClutterRect
 *
 * Frees the resources allocated by @rect.
 *
 * Since: 3.12
 */
void
clutter_rect_free (ClutterRect *rect)
{
  if (rect != NULL && rect != &_clutter_rect_zero)
    g_slice_free (ClutterRect, rect);
}

/**
 * clutter_rect_equal:
 * @a: a #ClutterRect
 * @b: a #ClutterRect
 *
 * Checks whether @a and @b are equals.
 *
 * This function will normalize both @a and @b before comparing
 * their origin and size.
 *
 * Returns: %TRUE if the rectangles match in origin and size.
 *
 * Since: 3.12
 */
gboolean
clutter_rect_equal (ClutterRect *a,
                    ClutterRect *b)
{
  if (a == b)
    return TRUE;

  if (a == NULL || b == NULL)
    return FALSE;

  clutter_rect_normalize_internal (a);
  clutter_rect_normalize_internal (b);

  return clutter_point_equal (&a->origin, &b->origin) &&
         clutter_size_equal (&a->size, &b->size);
}

/**
 * clutter_rect_normalize:
 * @rect: a #ClutterRect
 *
 * Normalizes a #ClutterRect.
 *
 * A #ClutterRect is defined by the area covered by its size; this means
 * that a #ClutterRect with #ClutterRect.origin in [ 0, 0 ] and a
 * #ClutterRect.size of [ 10, 10 ] is equivalent to a #ClutterRect with
 * #ClutterRect.origin in [ 10, 10 ] and a #ClutterRect.size of [ -10, -10 ].
 *
 * This function is useful to ensure that a rectangle has positive width
 * and height; it will modify the passed @rect and normalize its size.
 *
 * Since: 3.12
 */
ClutterRect *
clutter_rect_normalize (ClutterRect *rect)
{
  g_return_val_if_fail (rect != NULL, NULL);

  clutter_rect_normalize_internal (rect);

  return rect;
}

/**
 * clutter_rect_get_center:
 * @rect: a #ClutterRect
 * @center: (out caller-allocates): a #ClutterPoint
 *
 * Retrieves the center of @rect, after normalizing the rectangle,
 * and updates @center with the correct coordinates.
 *
 * Since: 3.12
 */
void
clutter_rect_get_center (ClutterRect  *rect,
                         ClutterPoint *center)
{
  g_return_if_fail (rect != NULL);
  g_return_if_fail (center != NULL);

  clutter_rect_normalize_internal (rect);

  center->x = rect->origin.x + (rect->size.width / 2.0f);
  center->y = rect->origin.y + (rect->size.height / 2.0f);
}

/**
 * clutter_rect_contains_point:
 * @rect: a #ClutterRect
 * @point: the point to check
 *
 * Checks whether @point is contained by @rect, after normalizing the
 * rectangle.
 *
 * Returns: %TRUE if the @point is contained by @rect.
 *
 * Since: 3.12
 */
gboolean
clutter_rect_contains_point (ClutterRect  *rect,
                             ClutterPoint *point)
{
  g_return_val_if_fail (rect != NULL, FALSE);
  g_return_val_if_fail (point != NULL, FALSE);

  clutter_rect_normalize_internal (rect);

  return (point->x >= rect->origin.x) &&
         (point->y >= rect->origin.y) &&
         (point->x <= (rect->origin.x + rect->size.width)) &&
         (point->y <= (rect->origin.y + rect->size.height));
}

/**
 * clutter_rect_contains_rect:
 * @a: a #ClutterRect
 * @b: a #ClutterRect
 *
 * Checks whether @a contains @b.
 *
 * The first rectangle contains the second if the union of the
 * two #ClutterRect is equal to the first rectangle.
 *
 * Returns: %TRUE if the first rectangle contains the second.
 *
 * Since: 3.12
 */
gboolean
clutter_rect_contains_rect (ClutterRect *a,
                            ClutterRect *b)
{
  ClutterRect res;

  g_return_val_if_fail (a != NULL, FALSE);
  g_return_val_if_fail (b != NULL, FALSE);

  clutter_rect_union (a, b, &res);

  return clutter_rect_equal (a, &res);
}

/**
 * clutter_rect_union:
 * @a: a #ClutterRect
 * @b: a #ClutterRect
 * @res: (out caller-allocates): a #ClutterRect
 *
 * Computes the smallest possible rectangle capable of fully containing
 * both @a and @b, and places it into @res.
 *
 * This function will normalize both @a and @b prior to computing their
 * union.
 *
 * Since: 3.12
 */
void
clutter_rect_union (ClutterRect *a,
                    ClutterRect *b,
                    ClutterRect *res)
{
  g_return_if_fail (a != NULL);
  g_return_if_fail (b != NULL);
  g_return_if_fail (res != NULL);

  clutter_rect_normalize_internal (a);
  clutter_rect_normalize_internal (b);

  res->origin.x = MIN (a->origin.x, b->origin.x);
  res->origin.y = MIN (a->origin.y, b->origin.y);

  res->size.width = MAX (a->size.width, b->size.width);
  res->size.height = MAX (a->size.height, b->size.height);
}

/**
 * clutter_rect_intersection:
 * @a: a #ClutterRect
 * @b: a #ClutterRect
 * @res: (out caller-allocates) (allow-none): a #ClutterRect, or %NULL
 *
 * Computes the intersection of @a and @b, and places it in @res, if @res
 * is not %NULL.
 *
 * This function will normalize both @a and @b prior to computing their
 * intersection.
 *
 * This function can be used to simply check if the intersection of @a and @b
 * is not empty, by using %NULL for @res.
 *
 * Returns: %TRUE if the intersection of @a and @b is not empty
 *
 * Since: 3.12
 */
gboolean
clutter_rect_intersection (ClutterRect *a,
                           ClutterRect *b,
                           ClutterRect *res)
{
  float x_1, y_1, x_2, y_2;

  g_return_val_if_fail (a != NULL, FALSE);
  g_return_val_if_fail (b != NULL, FALSE);

  clutter_rect_normalize_internal (a);
  clutter_rect_normalize_internal (b);

  x_1 = MAX (a->origin.x, b->origin.x);
  y_1 = MAX (a->origin.y, b->origin.y);
  x_2 = MIN (a->origin.x + a->size.width, b->origin.x + b->size.width);
  y_2 = MIN (a->origin.y + a->size.height, b->origin.y + b->size.height);

  if (x_1 >= x_2 || y_1 >= y_2)
    {
      if (res != NULL)
        clutter_rect_init (res, 0.f, 0.f, 0.f, 0.f);

      return FALSE;
    }

  if (res != NULL)
    clutter_rect_init (res, x_1, y_1, x_2 - x_1, y_2 - y_1);

  return TRUE;
}

/**
 * clutter_rect_offset:
 * @rect: a #ClutterRect
 * @d_x: the horizontal offset value
 * @d_y: the vertical offset value
 *
 * Offsets the origin of @rect by the given values, after normalizing
 * the rectangle.
 *
 * Since: 3.12
 */
void
clutter_rect_offset (ClutterRect *rect,
                     float        d_x,
                     float        d_y)
{
  g_return_if_fail (rect != NULL);

  clutter_rect_normalize_internal (rect);

  rect->origin.x += d_x;
  rect->origin.y += d_y;
}

/**
 * clutter_rect_inset:
 * @rect: a #ClutterRect
 * @d_x: an horizontal value; a positive @d_x will create an inset rectangle,
 *   and a negative value will create a larger rectangle
 * @d_y: a vertical value; a positive @d_x will create an inset rectangle,
 *   and a negative value will create a larger rectangle
 *
 * Normalizes the @rect and offsets its origin by the @d_x and @d_y values;
 * the size is adjusted by (2 * @d_x, 2 * @d_y).
 *
 * If @d_x and @d_y are positive the size of the rectangle is decreased; if
 * the values are negative, the size of the rectangle is increased.
 *
 * If the resulting rectangle has a negative width or height, the size is
 * set to 0.
 *
 * Since: 3.12
 */
void
clutter_rect_inset (ClutterRect *rect,
                    float        d_x,
                    float        d_y)
{
  g_return_if_fail (rect != NULL);

  clutter_rect_normalize_internal (rect);

  rect->origin.x += d_x;
  rect->origin.y += d_y;

  if (d_x >= 0.f)
    rect->size.width -= (d_x * 2.f);
  else
    rect->size.width += (d_x * -2.f);

  if (d_y >= 0.f)
    rect->size.height -= (d_y * 2.f);
  else
    rect->size.height += (d_y * -2.f);

  if (rect->size.width < 0.f)
    rect->size.width = 0.f;

  if (rect->size.height < 0.f)
    rect->size.height = 0.f;
}

/**
 * clutter_rect_clamp_to_pixel:
 * @rect: a #ClutterRect
 *
 * Rounds the origin of @rect downwards to the nearest integer, and rounds
 * the size of @rect upwards to the nearest integer, so that @rect is
 * updated to the smallest rectangle capable of fully containing the
 * original, fractional rectangle.
 *
 * Since: 3.12
 */
void
clutter_rect_clamp_to_pixel (ClutterRect *rect)
{
  g_return_if_fail (rect != NULL);

  clutter_rect_normalize_internal (rect);

  rect->origin.x = floorf (rect->origin.x);
  rect->origin.y = floorf (rect->origin.y);

  rect->size.width = ceilf (rect->size.width);
  rect->size.height = ceilf (rect->size.height);
}

/**
 * clutter_rect_get_x:
 * @rect: a #ClutterRect
 *
 * Retrieves the X coordinate of the origin of @rect.
 *
 * Returns: the X coordinate of the origin of the rectangle
 *
 * Since: 3.12
 */
float
clutter_rect_get_x (ClutterRect *rect)
{
  g_return_val_if_fail (rect != NULL, 0.f);

  clutter_rect_normalize_internal (rect);

  return rect->origin.x;
}

/**
 * clutter_rect_get_y:
 * @rect: a #ClutterRect
 *
 * Retrieves the Y coordinate of the origin of @rect.
 *
 * Returns: the Y coordinate of the origin of the rectangle
 *
 * Since: 3.12
 */
float
clutter_rect_get_y (ClutterRect *rect)
{
  g_return_val_if_fail (rect != NULL, 0.f);

  clutter_rect_normalize_internal (rect);

  return rect->origin.y;
}

/**
 * clutter_rect_get_width:
 * @rect: a #ClutterRect
 *
 * Retrieves the width of @rect.
 *
 * Returns: the width of the rectangle
 *
 * Since: 3.12
 */
float
clutter_rect_get_width (ClutterRect *rect)
{
  g_return_val_if_fail (rect != NULL, 0.f);

  clutter_rect_normalize_internal (rect);

  return rect->size.width;
}

/**
 * clutter_rect_get_height:
 * @rect: a #ClutterRect
 *
 * Retrieves the height of @rect.
 *
 * Returns: the height of the rectangle
 *
 * Since: 3.12
 */
float
clutter_rect_get_height (ClutterRect *rect)
{
  g_return_val_if_fail (rect != NULL, 0.f);

  clutter_rect_normalize_internal (rect);

  return rect->size.height;
}

/**
 * clutter_rect_interpolate:
 * @r1: a #ClutterRect
 * @r2: a #ClutterRect
 * @factor: the interpolation factor
 * @res: (out) (caller allocates): return location for the
 *   interpolated #ClutterRect
 *
 * Performs a linear interpolation of the the #ClutterRect.origin and
 * #ClutterRect.size of @r1 and @r2 using the interpolation @factor,
 * and places the resulting #ClutterRect inside @res.
 *
 * Since: 3.12
 */
void
clutter_rect_interpolate (const ClutterRect *r1,
                          const ClutterRect *r2,
                          double             factor,
                          ClutterRect       *res)
{
  g_return_if_fail (r1 != NULL);
  g_return_if_fail (r2 != NULL);
  g_return_if_fail (res != NULL);

  clutter_point_interpolate (&r1->origin, &r2->origin, factor, &res->origin);
  clutter_size_interpolate (&r1->size, &r2->size, factor, &res->size);

  clutter_rect_normalize_internal (res);
}

