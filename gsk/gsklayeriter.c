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

#include "gsklayerprivate.h"

static GskLayerIter *   gsk_layer_iter_copy     (GskLayerIter *iter);
static void             gsk_layer_iter_free     (GskLayerIter *iter);

G_DEFINE_BOXED_TYPE (GskLayerIter, gsk_layer_iter, gsk_layer_iter_copy, gsk_layer_iter_free)

/**
 * gsk_layer_iter_init:
 * @iter: the #GskLayerIter to initialize
 * @root: a #GskLayer
 *
 * Initializes a #GskLayerIter with a #GskLayer as its @root.
 *
 * The initialized iterator can be used to traverse the children of
 * the @root layer. You typically use a #GskLayerIter like this:
 *
 * |[<!-- language="C" -->
 *   GskLayerIter iter;
 *   GskLayer *child;
 *
 *   gsk_layer_iter_init (&iter, layer);
 *   while (gsk_layer_iter_next (&iter, &child))
 *     {
 *       // ...
 *     }
 * ]|
 *
 * Modifying the list of children of @root while iterating will
 * invalidate the iterator. Use gsk_layer_iter_remove() to safely
 * remove children while iterating.
 *
 * Since: 3.18
 */
void
gsk_layer_iter_init (GskLayerIter *iter,
                     GskLayer     *root)
{
  g_return_if_fail (iter != NULL);
  g_return_if_fail (GSK_IS_LAYER (root));

  iter->root = root;
  iter->root_age = gsk_layer_get_age (iter->root);
  iter->current = NULL;
}

/**
 * gsk_layer_iter_is_valid:
 * @iter: a #GskLayerIter
 *
 * Checks whether @iter is valid or not.
 *
 * A #GskLayerIter is valid if:
 *
 *  - it has been initialized with a #GskLayer
 *  - the list of children of #GskLayer hasn't changed
 *
 * Returns: %TRUE if the iterator is valid
 *
 * Since: 3.18
 */
gboolean
gsk_layer_iter_is_valid (GskLayerIter *iter)
{
  if (iter == NULL)
    return FALSE;

  if (iter->root == NULL)
    return FALSE;

  return gsk_layer_get_age (iter->root) == iter->root_age;
}

/**
 * gsk_layer_iter_next:
 * @iter: a #GskLayerIter
 * @child: (out) (transfer none): the next layer in the list of children
 *
 * Advances the iterator to the next child of the #GskLayer used to
 * initialize the #GskLayerIter.
 *
 * If the iterator successfully advanced, this function returns %TRUE and
 * the child #GskLayer.
 *
 * If the iterator could not advance, this function returns %FALSE.
 *
 * Returns: %TRUE if the iterator advanced
 *
 * Since: 3.18
 */
gboolean
gsk_layer_iter_next (GskLayerIter  *iter,
                     GskLayer     **child)
{
  g_return_val_if_fail (gsk_layer_iter_is_valid (iter), FALSE);

  if (iter->current == NULL)
    iter->current = gsk_layer_get_first_child (iter->root);
  else
    iter->current = gsk_layer_get_next_sibling (iter->current);

  if (child != NULL)
    *child = iter->current;

  return iter->current != NULL;
}

/**
 * gsk_layer_iter_prev:
 * @iter: a #GskLayerIte
 * @child: (out) (transfer none): the previous layer in the list of children
 *
 * Advances the iterator to the previous child of the #GskLayer used to
 * initialize the #GskLayerIter.
 *
 * If the iterator successfully advanced, this function returns %TRUE and
 * the child #GskLayer.
 *
 * If the iterator could not advance, this function returns %FALSE.
 *
 * Returns: %TRUE if the iterator advanced
 *
 * Since: 3.18
 */
gboolean
gsk_layer_iter_prev (GskLayerIter  *iter,
                     GskLayer     **child)
{
  g_return_val_if_fail (gsk_layer_iter_is_valid (iter), FALSE);

  if (iter->current == NULL)
    iter->current = gsk_layer_get_last_child (iter->root);
  else
    iter->current = gsk_layer_get_prev_sibling (iter->current);

  if (child != NULL)
    *child = iter->current;

  return iter->current != NULL;
}

/**
 * gsk_layer_iter_remove:
 * @iter: a #GskLayerIter
 *
 * Safely removes the #GskLayer currently pointed by a #GskLayerIter
 * from the list of children of the layer used to initialize the @iter.
 *
 * This function can only be called after gsk_layer_iter_next() or
 * gsk_layer_iter_prev() returned %TRUE.
 *
 * This function will call gsk_layer_remove_child() internally.
 *
 * Since: 3.18
 */
void
gsk_layer_iter_remove (GskLayerIter *iter)
{
  GskLayer *cur;

  g_return_if_fail (gsk_layer_iter_is_valid (iter));

  cur = iter->current;
  if (cur != NULL)
    {
      iter->current = gsk_layer_get_prev_sibling (cur);

      gsk_layer_remove_child (iter->root, cur);

      /* Increase the age to match remove_child() */
      iter->root_age += 1;
    }
}

static GskLayerIter *
gsk_layer_iter_copy (GskLayerIter *src)
{
  return g_slice_dup (GskLayerIter, src);
}

static void
gsk_layer_iter_free (GskLayerIter *iter)
{
  g_slice_free (GskLayerIter, iter);
}
