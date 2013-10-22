#ifndef __CLUTTER_ACTOR_H__
#define __CLUTTER_ACTOR_H__

#if !defined (__CLUTTER_H_INSIDE__) && !defined (CLUTTER_COMPILATION)
#error "Only <clutter/clutter.h> can be included directly."
#endif

#include <clutter/cluttertypes.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_ACTOR              (clutter_actor_get_type ())
#define CLUTTER_ACTOR(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_ACTOR, ClutterActor))
#define CLUTTER_IS_ACTOR(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_ACTOR))
#define CLUTTER_ACTOR_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_TYPE_ACTOR, ClutterActorClass))
#define CLUTTER_IS_ACTOR_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_TYPE_ACTOR))
#define CLUTTER_ACTOR_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_TYPE_ACTOR, ClutterActorClass))

typedef struct _ClutterActorClass       ClutterActorClass;
typedef struct _ClutterActorIter        ClutterActorIter;

/**
 * ClutterActor:
 *
 * The base element of the scene graph.
 *
 * Since: 3.12
 */
struct _ClutterActor
{
  /*< private >*/
  GInitiallyUnowned parent_instance;
};

struct _ClutterActorClass
{
  /*< private >*/
  GInitiallyUnownedClass parent_class;

  gpointer _padding[32];
};

CLUTTER_AVAILABLE_IN_3_12
GType clutter_actor_get_type (void);

CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_new                       (void);

/* Scene graph */
CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_add_child                 (ClutterActor *actor,
                                                                 ClutterActor *child);
CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_get_parent                (ClutterActor *actor);
CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_get_first_child           (ClutterActor *actor);
CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_get_last_child            (ClutterActor *actor);
CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_get_next_sibling          (ClutterActor *actor);
CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_get_previous_sibling      (ClutterActor *actor);
CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_insert_child_at_index     (ClutterActor *actor,
                                                                 ClutterActor *child,
                                                                 int           index_);
CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_insert_child_after        (ClutterActor *actor,
                                                                 ClutterActor *child,
                                                                 ClutterActor *sibling);
CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_insert_child_before       (ClutterActor *actor,
                                                                 ClutterActor *child,
                                                                 ClutterActor *sibling);
CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_replace_child             (ClutterActor *actor,
                                                                 ClutterActor *old_child,
                                                                 ClutterActor *new_child);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_actor_remove_child              (ClutterActor *actor,
                                                                 ClutterActor *child);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_actor_remove_all_children       (ClutterActor *actor);
CLUTTER_AVAILABLE_IN_3_12
int                     clutter_actor_get_n_children            (ClutterActor *actor);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_actor_set_child_at_index        (ClutterActor *actor,
                                                                 ClutterActor *child,
                                                                 int           index_);
CLUTTER_AVAILABLE_IN_3_12
ClutterActor *          clutter_actor_get_child_at_index        (ClutterActor *actor,
                                                                 int           index_);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_actor_set_child_after           (ClutterActor *actor,
                                                                 ClutterActor *child,
                                                                 ClutterActor *sibling);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_actor_set_child_before          (ClutterActor *actor,
                                                                 ClutterActor *child,
                                                                 ClutterActor *sibling);
CLUTTER_AVAILABLE_IN_3_12
gboolean                clutter_actor_contains                  (ClutterActor *actor,
                                                                 ClutterActor *child);

/* General */
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_actor_set_name                  (ClutterActor *actor,
                                                                 const char   *name);
CLUTTER_AVAILABLE_IN_3_12
const char *            clutter_actor_get_name                  (ClutterActor *actor);

/**
 * ClutterActorIter:
 *
 * An opaque structure that allows efficient iteration over the
 * children of a #ClutterActor.
 *
 * The contents of the <structname>ClutterActorIter</structname> structure
 * are private and should only be accessed using the provided API.
 *
 * Since: 3.12
 */
struct _ClutterActorIter
{
  /*< private >*/
  CLUTTER_PRIVATE_FIELD (ClutterActor *, root);
  CLUTTER_PRIVATE_FIELD (ClutterActor *, current);
  CLUTTER_PRIVATE_FIELD (gint64, age);
  CLUTTER_PRIVATE_FIELD (gpointer, padding1);
  CLUTTER_PRIVATE_FIELD (gpointer, padding2);
};

/* Iterator */
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_actor_iter_init                 (ClutterActorIter  *iter,
                                                                 ClutterActor      *actor);
CLUTTER_AVAILABLE_IN_3_12
gboolean                clutter_actor_iter_next                 (ClutterActorIter  *iter,
                                                                 ClutterActor     **child);
CLUTTER_AVAILABLE_IN_3_12
gboolean                clutter_actor_iter_prev                 (ClutterActorIter  *iter,
                                                                 ClutterActor     **child);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_actor_iter_remove_child         (ClutterActorIter  *iter);
CLUTTER_AVAILABLE_IN_3_12
gboolean                clutter_actor_iter_is_valid             (ClutterActorIter  *iter);

G_END_DECLS

#endif /* __CLUTTER_ACTOR_H__ */
