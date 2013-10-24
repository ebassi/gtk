#include "config.h"

#include <math.h>

#include "cluttertypes.h"

#define FLOAT_EPSILON	(1e-15)

static ClutterSize *	clutter_size_copy	(ClutterSize *size);

G_DEFINE_BOXED_TYPE (ClutterSize, clutter_size, clutter_size_copy, clutter_size_free)

/**
 * clutter_size_alloc: (constructor)
 *
 * Allocates a new #ClutterSize.
 *
 * Returns: (transfer full): the newly allocated #ClutterSize.
 *   Use clutter_size_free() to free its resources.
 *
 * Since: 3.12
 */
ClutterSize *
clutter_size_alloc (void)
{
  return g_slice_new0 (ClutterSize);
}

/**
 * clutter_size_init:
 * @size: a #ClutterSize
 * @width: the width
 * @height: the height
 *
 * Initializes a #ClutterSize with the given dimensions.
 *
 * Returns: (transfer none): the initialized #ClutterSize
 *
 * Since: 3.12
 */
ClutterSize *
clutter_size_init (ClutterSize *size,
                   float        width,
                   float        height)
{
  g_return_val_if_fail (size != NULL, NULL);

  size->width = width;
  size->height = height;

  return size;
}

/**
 * clutter_size_init:
 * @size: a #ClutterSize
 * @src: a #ClutterSize, used to initialize @size
 *
 * Initializes a #ClutterSize with the dimensions of the
 * given #ClutterSize.
 *
 * Returns: (transfer none): the initialized #ClutterSize
 *
 * Since: 3.12
 */
ClutterSize *
clutter_size_init_with_size (ClutterSize       *size,
                             const ClutterSize *src)
{
  g_return_val_if_fail (size != NULL, NULL);
  g_return_val_if_fail (src != NULL, size);

  *size = *src;

  return size;
}

static ClutterSize *
clutter_size_copy (ClutterSize *size)
{
  if (G_LIKELY (size != NULL))
    return g_slice_dup (ClutterSize, size);

  return NULL;
}

/**
 * clutter_size_free:
 * @size: a #ClutterSize
 *
 * Frees the resources allocated for @size.
 *
 * Since: 3.12
 */
void
clutter_size_free (ClutterSize *size)
{
  if (size != NULL)
    g_slice_free (ClutterSize, size);
}

/**
 * clutter_size_equal:
 * @v1: a #ClutterSize to compare
 * @v2: a #ClutterSize to compare
 *
 * Compares two #ClutterSize for equality.
 *
 * Returns: %TRUE if the two #ClutterSize are equal
 *
 * Since: 3.12
 */
gboolean
clutter_size_equal (gconstpointer v1,
                    gconstpointer v2)
{
  const ClutterSize *a = v1;
  const ClutterSize *b = v2;

  if (a == b)
    return TRUE;

  if (a == NULL || b == NULL)
    return FALSE;

  return fabsf (a->width - b->width) < FLOAT_EPSILON &&
         fabsf (a->height - b->height) < FLOAT_EPSILON;
}

/**
 * clutter_size_interpolate:
 * @s1: a #ClutterSize
 * @s2: a #ClutterSize
 * @factor: the interpolation factor
 * @res: (out) (caller allocates): return location for the
 *   interpolated #ClutterSize
 *
 * Performs a linear interpolation of the the #ClutterSize.width and
 * #ClutterSize.height of @s1 and @s2 using the interpolation @factor,
 * and places the resulting #ClutterRect inside @res.
 *
 * Since: 3.12
 */
void
clutter_size_interpolate (const ClutterSize *s1,
                          const ClutterSize *s2,
                          double             factor,
                          ClutterSize       *res)
{
  g_return_if_fail (s1 != NULL);
  g_return_if_fail (s2 != NULL);
  g_return_if_fail (res != NULL);

  res->width = s1->width + (s2->width - s1->width) * factor;
  res->height = s1->height + (s2->height - s1->height) * factor;
}
