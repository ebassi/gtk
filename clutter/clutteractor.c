/**
 * SECTION:clutteractor
 * @Title: ClutterActor
 * @Short_Description: The base element of the scene graph
 *
 * #ClutterActor is the base element of the scene graph.
 */

#include "config.h"

#include "clutteractorprivate.h"

#define CLUTTER_ACTOR_PRIVATE(obj)      ((ClutterActorPrivate *) (clutter_actor_get_instance_private (((ClutterActor *) obj))))

typedef struct _ClutterActorPrivate     ClutterActorPrivate;

struct _ClutterActorPrivate
{
  /* the actual scene graph */
  ClutterActor *parent;
  ClutterActor *first_child;
  ClutterActor *last_child;
  ClutterActor *previous_sibling;
  ClutterActor *next_sibling;

  int n_children;

  char *name;

  /* modified by add_child_internal/remove_child_internal, and
   * checked by the ClutterActorIter functions
   */
  gint64 age;
};

G_DEFINE_TYPE_WITH_PRIVATE (ClutterActor, clutter_actor, G_TYPE_INITIALLY_UNOWNED)

static const char *
clutter_actor_get_debug_name (ClutterActor *self)
{
  ClutterActorPrivate *priv = clutter_actor_get_instance_private (self);

  return priv->name != NULL ? priv->name : "<unknown>";
}

static gboolean
clutter_actor_check_parent_set (ClutterActor *actor,
                                ClutterActor *child)
{
  ClutterActorPrivate *child_priv = clutter_actor_get_instance_private (child);

  if (child_priv->parent != actor)
    {
      g_critical ("The actor '%s' of type '%s' is not a child of "
                  "the actor '%s' of type '%s'.",
                  clutter_actor_get_name (child), G_OBJECT_TYPE_NAME (child),
                  clutter_actor_get_name (actor), G_OBJECT_TYPE_NAME (actor));
      return FALSE;
    }

  return TRUE;
}

static gboolean
clutter_actor_check_parent_unset (ClutterActor *actor,
                                  ClutterActor *child)
{
  ClutterActorPrivate *child_priv = clutter_actor_get_instance_private (child);

  if (child_priv->parent != NULL)
    {
      g_critical ("The actor '%s' of type '%s' already has a parent "
                  "actor '%s' of type '%s', and cannot be added to the "
                  "actor '%s' of type '%s'.",
                  clutter_actor_get_debug_name (child), G_OBJECT_TYPE_NAME (child),
                  clutter_actor_get_debug_name (child_priv->parent), G_OBJECT_TYPE_NAME (child_priv->parent),
                  clutter_actor_get_debug_name (actor), G_OBJECT_TYPE_NAME (actor));
      return FALSE;
    }

  return TRUE;
}

static gboolean
clutter_actor_check_sibling (ClutterActor *actor,
                             ClutterActor *child,
                             ClutterActor *sibling)
{
  ClutterActorPrivate *sibling_priv = clutter_actor_get_instance_private (sibling);

  if (sibling_priv->parent != actor)
    {
      g_critical ("The actor '%s' of type '%s' is not a child of "
                  "the actor '%s' of type '%s', and cannot be "
                  "used a sibling of the actor '%s' of type '%s'.",
                  clutter_actor_get_debug_name (sibling), G_OBJECT_TYPE_NAME (sibling),
                  clutter_actor_get_debug_name (actor), G_OBJECT_TYPE_NAME (actor),
                  clutter_actor_get_debug_name (child), G_OBJECT_TYPE_NAME (child));
      return FALSE;
    }

  return TRUE;
}

static void
clutter_actor_dispose (GObject *gobject)
{
  clutter_actor_remove_all_children (CLUTTER_ACTOR (gobject));

  G_OBJECT_CLASS (clutter_actor_parent_class)->dispose (gobject);
}

static void
clutter_actor_finalize (GObject *gobject)
{
  ClutterActorPrivate *priv = CLUTTER_ACTOR_PRIVATE (gobject);

  g_free (priv->name);

  G_OBJECT_CLASS (clutter_actor_parent_class)->finalize (gobject);
}

static void
clutter_actor_class_init (ClutterActorClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = clutter_actor_dispose;
  gobject_class->finalize = clutter_actor_finalize;
}

static void
clutter_actor_init (ClutterActor *self)
{
}

/**
 * clutter_actor_new:
 *
 * Creates a new #ClutterActor.
 *
 * Returns: (transfer full): the newly created #ClutterActor
 *
 * Since: 3.12
 */
ClutterActor *
clutter_actor_new (void)
{
  return g_object_new (CLUTTER_TYPE_ACTOR, NULL);
}

typedef enum {
  REMOVE_CHILD_EMIT_PARENT_SET          = 1 << 0,
  REMOVE_CHILD_EMIT_ACTOR_REMOVED       = 1 << 1,
  REMOVE_CHILD_CHECK_STATE              = 1 << 2,

  CLUTTER_ACTOR_REMOVE_DEFAULT_FLAGS = REMOVE_CHILD_EMIT_PARENT_SET |
                                       REMOVE_CHILD_EMIT_ACTOR_REMOVED |
                                       REMOVE_CHILD_CHECK_STATE
} ClutterActorRemoveChildFlags;

static inline void
clutter_actor_remove_child_internal (ClutterActor                 *actor,
                                     ClutterActor                 *child,
                                     ClutterActorRemoveChildFlags  flags)
{
  ClutterActorPrivate *priv = clutter_actor_get_instance_private (actor);
  ClutterActorPrivate *child_priv = clutter_actor_get_instance_private (child);
  ClutterActor *prev, *next;

  prev = child_priv->previous_sibling;
  next = child_priv->next_sibling;

  if (prev != NULL)
    CLUTTER_ACTOR_PRIVATE (prev)->next_sibling = next;

  if (next != NULL)
    CLUTTER_ACTOR_PRIVATE (next)->previous_sibling = prev;

  if (priv->first_child == child)
    priv->first_child = next;

  if (priv->last_child == child)
    priv->last_child = prev;

  child_priv->parent = NULL;
  child_priv->previous_sibling = NULL;
  child_priv->next_sibling = NULL;

  priv->age += 1;
  priv->n_children -= 1;

  /* release the reference acquired in clutter_actor_add_child_internal() */
  g_object_unref (child);
}

typedef enum {
  ADD_CHILD_EMIT_PARENT_SET             = 1 << 0,
  ADD_CHILD_EMIT_ACTOR_ADDED            = 1 << 1,
  ADD_CHILD_CHECK_STATE                 = 1 << 2,

  CLUTTER_ACTOR_ADD_DEFAULT_FLAGS = ADD_CHILD_EMIT_PARENT_SET |
                                    ADD_CHILD_EMIT_ACTOR_ADDED |
                                    ADD_CHILD_CHECK_STATE
} ClutterActorAddChildFlags;

typedef void (* ClutterActorAddChildFunc) (ClutterActor *self,
                                           ClutterActor *child,
                                           gpointer      data);

static inline void
clutter_actor_add_child_internal (ClutterActor              *self,
                                  ClutterActor              *child,
                                  ClutterActorAddChildFlags  flags,
                                  ClutterActorAddChildFunc   add_func,
                                  gpointer                   add_data)
{
  ClutterActorPrivate *priv = clutter_actor_get_instance_private (self);
  ClutterActorPrivate *child_priv = clutter_actor_get_instance_private (child);

  g_object_freeze_notify (G_OBJECT (self));

  g_assert (child_priv->parent == NULL);

  /* own the child */
  g_object_ref_sink (child);
  child_priv->parent = self;

  /* delegate the insertion policy */
  add_func (self, child, add_data);

  priv->n_children += 1;
  priv->age += 1;

  if (child_priv->previous_sibling == NULL)
    priv->first_child = child;

  if (child_priv->next_sibling == NULL)
    priv->last_child = child;

  g_object_thaw_notify (G_OBJECT (self));

  /* sanity check */
  g_assert (child_priv->parent == self);
  g_assert (priv->first_child != NULL);
  g_assert (priv->last_child != NULL);
  g_assert (priv->n_children != 0);
}

static void
insert_child_after (ClutterActor *self,
                    ClutterActor *child,
                    gpointer      data)
{
  ClutterActor *sibling = data;

  CLUTTER_ACTOR_PRIVATE (child)->parent = self;

  if (sibling == NULL)
    sibling = CLUTTER_ACTOR_PRIVATE (self)->last_child;

  CLUTTER_ACTOR_PRIVATE (child)->previous_sibling = sibling;

  if (sibling != NULL)
    {
      ClutterActor *tmp = CLUTTER_ACTOR_PRIVATE (sibling)->next_sibling;

      CLUTTER_ACTOR_PRIVATE (child)->next_sibling = tmp;

      if (tmp != NULL)
        CLUTTER_ACTOR_PRIVATE (tmp)->previous_sibling = child;

      CLUTTER_ACTOR_PRIVATE (sibling)->next_sibling = child;
    }
  else
    {
      CLUTTER_ACTOR_PRIVATE (child)->next_sibling = NULL;
    }
}

static void
insert_child_before (ClutterActor *self,
                     ClutterActor *child,
                     gpointer      data)
{
  ClutterActor *sibling = data;

  CLUTTER_ACTOR_PRIVATE (child)->parent = self;

  if (sibling == NULL)
    sibling = CLUTTER_ACTOR_PRIVATE (self)->first_child;

  CLUTTER_ACTOR_PRIVATE (child)->next_sibling = sibling;

  if (sibling != NULL)
    {
      ClutterActor *tmp = CLUTTER_ACTOR_PRIVATE (sibling)->previous_sibling;

      CLUTTER_ACTOR_PRIVATE (child)->previous_sibling = tmp;

      if (tmp != NULL)
        CLUTTER_ACTOR_PRIVATE (tmp)->next_sibling = child;

      CLUTTER_ACTOR_PRIVATE (sibling)->previous_sibling = child;
    }
  else
    {
      CLUTTER_ACTOR_PRIVATE (child)->previous_sibling = NULL;
    }
}

struct BetweenData
{
  ClutterActor *prev;
  ClutterActor *next;
};

static void
insert_child_between (ClutterActor *self,
                      ClutterActor *child,
                      gpointer      data_)
{
  struct BetweenData *data = data_;
  ClutterActor *prev_sibling = data->prev;
  ClutterActor *next_sibling = data->next;

  CLUTTER_ACTOR_PRIVATE (child)->parent = self;
  CLUTTER_ACTOR_PRIVATE (child)->previous_sibling = prev_sibling;
  CLUTTER_ACTOR_PRIVATE (child)->next_sibling = next_sibling;

  if (prev_sibling != NULL)
    CLUTTER_ACTOR_PRIVATE (prev_sibling)->next_sibling = child;

  if (next_sibling != NULL)
    CLUTTER_ACTOR_PRIVATE (next_sibling)->previous_sibling = child;
}

static void
insert_child_at_index (ClutterActor *self,
                       ClutterActor *child,
                       gpointer      data)
{
  ClutterActorPrivate *priv = clutter_actor_get_instance_private (self);
  int position = GPOINTER_TO_INT (data);

  if (position == 0)
    {
      ClutterActor *tmp = priv->first_child;

      if (tmp != NULL)
        CLUTTER_ACTOR_PRIVATE (tmp)->previous_sibling = child;

      CLUTTER_ACTOR_PRIVATE (child)->previous_sibling = NULL;
      CLUTTER_ACTOR_PRIVATE (child)->next_sibling = tmp;
    }
  else if (position < 0 || position >= priv->n_children)
    {
      ClutterActor *tmp = priv->last_child;

      if (tmp != NULL)
        CLUTTER_ACTOR_PRIVATE (tmp)->next_sibling = child;

      CLUTTER_ACTOR_PRIVATE (child)->previous_sibling = tmp;
      CLUTTER_ACTOR_PRIVATE (child)->next_sibling = NULL;
    }
  else
    {
      ClutterActor *iter;
      int i;

      iter = priv->first_child;
      for (i = 0; i < priv->n_children; i += 1)
        {
          if (i == position)
            {
              ClutterActor *tmp = CLUTTER_ACTOR_PRIVATE (iter)->previous_sibling;

              CLUTTER_ACTOR_PRIVATE (child)->previous_sibling = tmp;
              CLUTTER_ACTOR_PRIVATE (child)->next_sibling = iter;

              if (tmp != NULL)
                CLUTTER_ACTOR_PRIVATE (tmp)->next_sibling = child;

              CLUTTER_ACTOR_PRIVATE (iter)->previous_sibling = child;

              break;
            }

          iter = CLUTTER_ACTOR_PRIVATE (iter)->next_sibling;
        }
    }
}

/**
 * clutter_actor_add_child:
 * @actor: a #ClutterActor
 * @child: a #ClutterActor to be added to @actor
 *
 * Appends @child to the list of children of @actor.
 *
 * This function acquires a reference to @child. Use clutter_actor_remove_child()
 * to release it.
 *
 * Returns: (transfer none): the modified @actor
 *
 * Since: 3.12
 */
ClutterActor *
clutter_actor_add_child (ClutterActor *actor,
                         ClutterActor *child)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), NULL);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), actor);

  /* add_child() behaves like insert_child_above(sibling=NULL) */
  clutter_actor_add_child_internal (actor, child,
                                    CLUTTER_ACTOR_ADD_DEFAULT_FLAGS,
                                    insert_child_after,
                                    NULL);

  return actor;
}

/**
 * clutter_actor_remove_child:
 * @actor: a #ClutterActor
 * @child: a #ClutterActor to remove
 *
 * Removes @child from the list of children of @actor.
 *
 * This function releases the reference acquired on @child by the
 * clutter_actor_add_child() and clutter_actor_insert_child_* functions.
 *
 * Since: 3.12
 */
void
clutter_actor_remove_child (ClutterActor *actor,
                            ClutterActor *child)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (actor));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));
  g_return_if_fail (clutter_actor_check_parent_set (actor, child));

  clutter_actor_remove_child_internal (actor, child, CLUTTER_ACTOR_REMOVE_DEFAULT_FLAGS);
}

/**
 * clutter_actor_get_parent:
 * @actor: a #ClutterActor
 *
 * Retrieves the parent of @actor in the scene graph.
 *
 * Returns: (transfer none): the parent of @actor
 *
 * Since: 3.12
 */
CLUTTER_ACTOR_DEFINE_GET (ClutterActor, clutter_actor, ClutterActor *, parent)

/**
 * clutter_actor_get_previous_sibling:
 * @actor: a #ClutterActor
 *
 * Retrieves the previous sibling of @actor in the scene graph.
 *
 * Returns: (transfer none): the previous sibling of @actor
 *
 * Since: 3.12
 */
CLUTTER_ACTOR_DEFINE_GET (ClutterActor, clutter_actor, ClutterActor *, previous_sibling)

/**
 * clutter_actor_get_next_sibling:
 * @actor: a #ClutterActor
 *
 * Retrieves the next sibling of @actor in the scene graph.
 *
 * Returns: (transfer none): the next sibling of @actor
 *
 * Since: 3.12
 */
CLUTTER_ACTOR_DEFINE_GET (ClutterActor, clutter_actor, ClutterActor *, next_sibling)

/**
 * clutter_actor_get_first_child:
 * @actor: a #ClutterActor
 *
 * Retrieves the first child of @actor.
 *
 * Returns: (transfer none): the first child of @actor
 *
 * Since: 3.12
 */
CLUTTER_ACTOR_DEFINE_GET (ClutterActor, clutter_actor, ClutterActor *, first_child)

/**
 * clutter_actor_get_last_child:
 * @actor: a #ClutterActor
 *
 * Retrieves the last child of @actor.
 *
 * Returns: (transfer none): the last child of @actor
 *
 * Since: 3.12
 */
CLUTTER_ACTOR_DEFINE_GET (ClutterActor, clutter_actor, ClutterActor *, last_child)

/**
 * clutter_actor_get_name:
 * @actor: a #ClutterActor
 *
 * Retrieves the name of @actor, as set by clutter_actor_set_name().
 *
 * Returns: (transfer none): the name of the @actor
 *
 * Since: 3.12
 */
CLUTTER_ACTOR_DEFINE_GET (ClutterActor, clutter_actor, const char *, name)

/**
 * clutter_actor_get_n_children:
 * @actor: a #ClutterActor
 *
 * Retrieves the number of children of @actor.
 *
 * Returns: the number of children of the @actor
 *
 * Since: 3.12
 */
CLUTTER_ACTOR_DEFINE_GET (ClutterActor, clutter_actor, int, n_children)

/**
 * clutter_actor_insert_child_at_index:
 * @actor: a #ClutterActor
 * @child: a #ClutterActor to be inserted at the given @index_
 * @index_: the position used to insert the @child
 *
 * Inserts @child in the list of children of @actor using the given @index_.
 *
 * If @index_ is a negative number, or bigger than the number of children
 * of @actor, the @child will be appended to the list of children.
 *
 * This function acquires a reference to @child. Use clutter_actor_remove_child()
 * to release it.
 *
 * Returns: (transfer none): the modified @actor
 *
 * Since: 3.12
 */
ClutterActor *
clutter_actor_insert_child_at_index (ClutterActor *actor,
                                     ClutterActor *child,
                                     int           index_)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), NULL);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), actor);

  clutter_actor_add_child_internal (actor, child,
                                    CLUTTER_ACTOR_ADD_DEFAULT_FLAGS,
                                    insert_child_at_index,
                                    GINT_TO_POINTER (index_));

  return actor;
}

/**
 * clutter_actor_insert_child_after:
 * @actor: a #ClutterActor
 * @child: a #ClutterActor to be inserted after @sibling
 * @sibling: (allow-none): a #ClutterActor child of @actor, or %NULL
 *
 * Inserts @child in the list of children of @actor, after @sibling.
 *
 * If @sibling is %NULL, the @child is appended to the list of children
 * of @actor.
 *
 * This function acquires a reference to @child. Use clutter_actor_remove_child()
 * to release it.
 *
 * Returns: (transfer none): the modified @actor
 *
 * Since: 3.12
 */
ClutterActor *
clutter_actor_insert_child_after (ClutterActor *actor,
                                  ClutterActor *child,
                                  ClutterActor *sibling)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), NULL);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), actor);
  g_return_val_if_fail (sibling == NULL || CLUTTER_IS_ACTOR (sibling), actor);

  if (sibling != NULL && !clutter_actor_check_sibling (actor, child, sibling))
    return actor;

  clutter_actor_add_child_internal (actor, child,
                                    CLUTTER_ACTOR_ADD_DEFAULT_FLAGS,
                                    insert_child_after,
                                    sibling);

  return actor;
}

/**
 * clutter_actor_insert_child_before:
 * @actor: a #ClutterActor
 * @child: a #ClutterActor to be inserted before @sibling
 * @sibling: (allow-none): a #ClutterActor child of @actor, or %NULL
 *
 * Inserts @child in the list of children of @actor, before @sibling.
 *
 * If @sibling is %NULL, @child is prepended to the list of children
 * of @actor.
 *
 * This function acquires a reference to @child. Use clutter_actor_remove_child()
 * to release it.
 *
 * Returns: (transfer none): the modified @actor
 *
 * Since: 3.12
 */
ClutterActor *
clutter_actor_insert_child_before (ClutterActor *actor,
                                   ClutterActor *child,
                                   ClutterActor *sibling)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), NULL);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (child), actor);
  g_return_val_if_fail (sibling == NULL || CLUTTER_IS_ACTOR (sibling), actor);

  if (sibling != NULL && !clutter_actor_check_sibling (actor, child, sibling))
    return actor;

  clutter_actor_add_child_internal (actor, child,
                                    CLUTTER_ACTOR_ADD_DEFAULT_FLAGS,
                                    insert_child_before,
                                    sibling);

  return actor;
}

/**
 * clutter_actor_replace_child:
 * @actor: a #ClutterActor
 * @old_child: a #ClutterActor child of @actor to be replaced by @new_child
 * @new_child: a #ClutterActor to replace @old_child
 *
 * Replaces @old_child with @new_child in the list of children of @actor.
 *
 * This function releases the reference acquired on @old_child, and acquires
 * a reference on @new_child.
 *
 * Returns: (transfer none): the modified @actor
 *
 * Since: 3.12
 */
ClutterActor *
clutter_actor_replace_child (ClutterActor *actor,
                             ClutterActor *old_child,
                             ClutterActor *new_child)
{
  ClutterActorPrivate *child_priv;
  struct BetweenData data;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), NULL);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (old_child), actor);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (new_child), actor);
  g_return_val_if_fail (clutter_actor_check_parent_set (actor, old_child), actor);
  g_return_val_if_fail (clutter_actor_check_parent_unset (actor, new_child), actor);

  child_priv = clutter_actor_get_instance_private (old_child);
  data.prev = child_priv->previous_sibling;
  data.next = child_priv->next_sibling;

  clutter_actor_remove_child_internal (actor, old_child, CLUTTER_ACTOR_REMOVE_DEFAULT_FLAGS);
  clutter_actor_add_child_internal (actor, new_child, CLUTTER_ACTOR_ADD_DEFAULT_FLAGS,
                                    insert_child_between,
                                    &data);

  return actor;
}

/**
 * clutter_actor_remove_all_children:
 * @actor: a #ClutterActor
 *
 * Removes all children of @actor.
 *
 * This function releases the reference acquired by clutter_actor_add_child()
 * and clutter_actor_insert_child_* functions.
 *
 * Since: 3.12
 */
void
clutter_actor_remove_all_children (ClutterActor *actor)
{
  ClutterActorPrivate *priv = clutter_actor_get_instance_private (actor);
  ClutterActorIter iter;

  g_return_if_fail (CLUTTER_IS_ACTOR (actor));

  if (priv->n_children == 0)
    return;

  g_object_freeze_notify (G_OBJECT (actor));

  clutter_actor_iter_init (&iter, actor);
  while (clutter_actor_iter_next (&iter, NULL))
    clutter_actor_iter_remove_child (&iter);

  g_object_thaw_notify (G_OBJECT (actor));

  /* sanity check */
  g_assert (priv->n_children == 0);
  g_assert (priv->first_child == NULL);
  g_assert (priv->last_child == NULL);
}

/**
 * clutter_actor_set_child_at_index:
 * @actor: a #ClutterActor
 * @child: a #ClutterActor child of @actor
 * @index_: the new position of @child in the list of children of @actor
 *
 * Sets @child to be at the given @index_ in the list of children
 * of @actor.
 *
 * If @index_ is negative, or greater than the number of children, then
 * the @child is moved at the end of the list of children.
 *
 * Since: 3.12
 */
void
clutter_actor_set_child_at_index (ClutterActor *actor,
                                  ClutterActor *child,
                                  int           index_)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (actor));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));
  g_return_if_fail (clutter_actor_check_parent_set (actor, child));

  g_object_ref (child);
  clutter_actor_remove_child_internal (actor, child, 0);
  clutter_actor_add_child_internal (actor, child,
                                    0,
                                    insert_child_at_index,
                                    GINT_TO_POINTER (index_));
  g_object_unref (child);
}

/**
 * clutter_actor_get_child_at_index:
 * @actor: a #ClutterActor
 * @index_: the index of the child to retrieve
 *
 * Retrieves the child of @actor at the given index.
 *
 * This function traverses the list of children, so it should not
 * be used while iterating over the children of @actor.
 *
 * Returns: (transfer none): the #ClutterActor at the given index,
 *   or %NULL if no child was found
 *
 * Since: 3.12
 */
ClutterActor *
clutter_actor_get_child_at_index (ClutterActor *actor,
                                  int           index_)
{
  ClutterActorPrivate *priv = clutter_actor_get_instance_private (actor);
  ClutterActor *iter;
  int i;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), NULL);

  if (index_ < 0)
    return NULL;

  if (index_ >= priv->n_children)
    return NULL;

  iter = priv->first_child;
  for (i = 0; i < index_ && iter != NULL; i++)
    {
      ClutterActorPrivate *iter_priv = clutter_actor_get_instance_private (iter);

      iter = iter_priv->next_sibling;
    }

  return iter;
}

/**
 * clutter_actor_set_child_after:
 * @actor: a #ClutterActor
 * @child: a #ClutterActor child of @actor
 * @sibling: (allow-none): a #ClutterActor child of @actor, or %NULL
 *
 * Moves @child to a position in the list of children of @actor that
 * comes after @sibling.
 *
 * If @sibling is %NULL, @child will be placed last in the list of
 * children.
 *
 * Since: 3.12
 */
void
clutter_actor_set_child_after (ClutterActor *actor,
                               ClutterActor *child,
                               ClutterActor *sibling)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (actor));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));
  g_return_if_fail (clutter_actor_check_parent_set (actor, child));
  g_return_if_fail (sibling == NULL || CLUTTER_IS_ACTOR (sibling));

  if (sibling != NULL && !clutter_actor_check_sibling (actor, child, sibling))
    return;

  g_object_ref (child);
  clutter_actor_remove_child_internal (actor, child, 0);
  clutter_actor_add_child_internal (actor, child, 0, insert_child_after, sibling);
  g_object_unref (child);
}

/**
 * clutter_actor_set_child_before:
 * @actor: a #ClutterActor
 * @child: a #ClutterActor child of @actor
 * @sibling: (allow-none): a #ClutterActor child of @actor, or %NULL
 *
 * Moves @child to a position in the list of children of @actor that
 * comes before @sibling.
 *
 * If @sibling is %NULL, @child will be placed first in the list of
 * children.
 *
 * Since: 3.12
 */
void
clutter_actor_set_child_before (ClutterActor *actor,
                                ClutterActor *child,
                                ClutterActor *sibling)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (actor));
  g_return_if_fail (CLUTTER_IS_ACTOR (child));
  g_return_if_fail (clutter_actor_check_parent_set (actor, child));
  g_return_if_fail (sibling == NULL || CLUTTER_IS_ACTOR (sibling));

  if (sibling != NULL && !clutter_actor_check_sibling (actor, child, sibling))
    return;

  g_object_ref (child);
  clutter_actor_remove_child_internal (actor, child, 0);
  clutter_actor_add_child_internal (actor, child, 0, insert_child_before, sibling);
  g_object_unref (child);
}

/**
 * clutter_actor_contains:
 * @actor: a #ClutterActor
 * @descendant: a #ClutterActor
 *
 * Checks whether @actor contains @descendant.
 *
 * Returns: %TRUE if @actor contains @descendant
 *
 * Since: 3.12
 */
gboolean
clutter_actor_contains (ClutterActor *actor,
                        ClutterActor *descendant)
{
  ClutterActor *iter;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), FALSE);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (descendant), FALSE);

  iter = descendant;
  while (iter != NULL)
    {
      ClutterActorPrivate *iter_priv = clutter_actor_get_instance_private (iter);

      if (iter == actor)
        return TRUE;

      iter = iter_priv->parent;
    }

  return FALSE;
}

/**
 * clutter_actor_set_name:
 * @actor: a #ClutterActor
 * @name: (allow-none): the name of the actor
 *
 * Sets the name of the @actor. The name of the actor is useful for
 * debugging purposes.
 *
 * There are no constraints on the @name string.
 *
 * Since: 3.12
 */
void
clutter_actor_set_name (ClutterActor *actor,
                        const char   *name)
{
  ClutterActorPrivate *priv = clutter_actor_get_instance_private (actor);
  char *new_name;

  g_return_if_fail (CLUTTER_IS_ACTOR (actor));

  new_name = g_strdup (name);
  g_free (priv->name);
  priv->name = new_name;
}

/**
 * clutter_actor_iter_init:
 * @iter: a #ClutterActorIter
 * @actor: a #ClutterActor
 *
 * Initializes a #ClutterActorIter, which can then be used to efficiently
 * iterate over the list of children of @actor. For instance:
 *
 * |[
 *   ClutterActorIter iter;
 *   ClutterActor *child;
 *
 *   clutter_actor_iter_init (&iter, root);
 *   while (clutter_actor_iter_next (&iter, &child))
 *     {
 *       /&ast; do something with child &ast;/
 *     }
 * ]|
 *
 * Changing the section of the scene graph that contains @actor while
 * iterating it invalidates the iterator.
 *
 * Since: 3.12
 */
void
clutter_actor_iter_init (ClutterActorIter *iter,
                         ClutterActor     *actor)
{
  g_return_if_fail (iter != NULL);
  g_return_if_fail (CLUTTER_IS_ACTOR (actor));

  iter->root = actor;
  iter->current = NULL;
  iter->age = CLUTTER_ACTOR_PRIVATE (actor)->age;
}

/**
 * clutter_actor_iter_is_valid:
 * @iter: a #ClutterActorIter
 *
 * Checks if @iter is valid.
 *
 * A #ClutterActorIter is considered valid if it has been initialized,
 * and if the list of children of the #ClutterActor used to initialize
 * the iterator hasn't been changed.
 *
 * Returns: %TRUE if the #ClutterActorIter is valid, and %FALSE otherwise
 *
 * Since: 3.12
 */
gboolean
clutter_actor_iter_is_valid (ClutterActorIter *iter)
{
  g_return_val_if_fail (iter != NULL, FALSE);

  if (iter->root == NULL)
    return FALSE;

  return iter->age == CLUTTER_ACTOR_PRIVATE (iter->root)->age;
}

/**
 * clutter_actor_iter_prev:
 * @iter: a valid #ClutterActorIter
 * @child: (out) (allow-none): return location for the child, or %NULL
 *
 * Moves the iterator and retrieves the previous child of the actor
 * used to initialize @iter.
 *
 * If the iterator was moved, this function returns %TRUE and puts the
 * actor at the address of @child.
 *
 * If the iterator could not be moved, this function returns %FALSE and
 * the contents of @child are undefined.
 *
 * Returns: %TRUE if the iterator was moved, and %FALSE otherwise
 *
 * Since: 3.12
 */
gboolean
clutter_actor_iter_next (ClutterActorIter  *iter,
                         ClutterActor     **child)
{
  g_return_val_if_fail (iter != NULL, FALSE);
  g_return_val_if_fail (iter->root != NULL, FALSE);
#ifdef G_DISABLE_ASSERT
  g_return_val_if_fail (clutter_actor_iter_is_valid (iter), FALSE);
#endif

  if (iter->current == NULL)
    iter->current = CLUTTER_ACTOR_PRIVATE (iter->root)->first_child;
  else
    iter->current = CLUTTER_ACTOR_PRIVATE (iter->current)->next_sibling;

  if (child != NULL)
    *child = iter->current;

  return iter->current != NULL;
}

/**
 * clutter_actor_iter_prev:
 * @iter: a valid #ClutterActorIter
 * @child: (out) (allow-none): return location for the child, or %NULL
 *
 * Moves the iterator and retrieves the previous child of the actor
 * used to initialize @iter.
 *
 * If the iterator was moved, this function returns %TRUE and puts the
 * actor at the address of @child.
 *
 * If the iterator could not be moved, this function returns %FALSE and
 * the contents of @child are undefined.
 *
 * Returns: %TRUE if the iterator was moved, and %FALSE otherwise
 *
 * Since: 3.12
 */
gboolean
clutter_actor_iter_prev (ClutterActorIter  *iter,
                         ClutterActor     **child)
{
  g_return_val_if_fail (iter != NULL, FALSE);
  g_return_val_if_fail (iter->root != NULL, FALSE);
#ifdef G_DISABLE_ASSERT
  g_return_val_if_fail (clutter_actor_iter_is_valid (iter), FALSE);
#endif

  if (iter->current == NULL)
    iter->current = CLUTTER_ACTOR_PRIVATE (iter->root)->last_child;
  else
    iter->current = CLUTTER_ACTOR_PRIVATE (iter->current)->previous_sibling;

  if (child != NULL)
    *child = iter->current;

  return iter->current != NULL;
}

/**
 * clutter_actor_iter_remove_child:
 * @iter: a #ClutterActorIter
 *
 * Safely removes the #ClutterActor currently pointed by @iter from its
 * parent, i.e. the #ClutterActor used to initialize the @iter.
 *
 * This function can only be called after clutter_actor_iter_next() or
 * clutter_actor_iter_prev() returned %TRUE, and cannot be called more
 * than once.
 *
 * This function will call clutter_actor_remove_child().
 *
 * Since: 3.12
 */
void
clutter_actor_iter_remove_child (ClutterActorIter *iter)
{
  ClutterActor *tmp;

  g_return_if_fail (iter != NULL);
  g_return_if_fail (iter->root != NULL);
#ifdef G_DISABLE_ASSERT
  g_return_if_fail (clutter_actor_iter_is_valid (iter));
#endif

  tmp = iter->current;

  if (tmp != NULL)
    {
      iter->current = CLUTTER_ACTOR_PRIVATE (tmp)->previous_sibling;

      clutter_actor_remove_child_internal (iter->root, tmp, CLUTTER_ACTOR_REMOVE_DEFAULT_FLAGS);

      iter->age += 1;
    }
}
