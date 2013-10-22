#include <clutter/clutter.h>

static void
traverse_graph_maybe_recurse (ClutterActor *actor,
                              GString      *buffer)
{
  g_string_append_printf (buffer, "[ %s ", clutter_actor_get_name (actor));

  if (clutter_actor_get_n_children (actor) != 0)
    {
      ClutterActorIter iter;
      ClutterActor *child;

      g_string_append (buffer, "[ ");

      clutter_actor_iter_init (&iter, actor);
      while (clutter_actor_iter_next (&iter, &child))
        traverse_graph_maybe_recurse (child, buffer);

     g_string_append (buffer, "] ");
    }

  g_string_append (buffer, "] ");
}

static char *
clutter_test_strdup_graph (ClutterActor *actor)
{
  GString *buffer;

  buffer = g_string_new (NULL);
  traverse_graph_maybe_recurse (actor, buffer);

  return g_string_free (buffer, FALSE);
}

static void
actor_graph_initial_state (void)
{
  ClutterActor *actor;

  actor = g_object_ref_sink (clutter_actor_new ());
  g_object_add_weak_pointer (G_OBJECT (actor), (gpointer *) &actor);

  g_assert_null (clutter_actor_get_parent (actor));
  g_assert_null (clutter_actor_get_first_child (actor));
  g_assert_null (clutter_actor_get_last_child (actor));
  g_assert_null (clutter_actor_get_next_sibling (actor));
  g_assert_null (clutter_actor_get_previous_sibling (actor));
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, 0);

  g_object_unref (actor);
  g_assert_null (actor);
}

static void
actor_graph_add_child (void)
{
  ClutterActor *actor, *child1, *child2;

  actor = g_object_ref_sink (clutter_actor_new ());
  g_object_add_weak_pointer (G_OBJECT (actor), (gpointer *) &actor);

  child1 = clutter_actor_new ();
  g_object_add_weak_pointer (G_OBJECT (child1), (gpointer *) &child1);

  clutter_actor_add_child (actor, child1);
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, 1);
  g_assert_true (clutter_actor_contains (actor, child1));
  g_assert (clutter_actor_get_first_child (actor) == child1);
  g_assert (clutter_actor_get_last_child (actor) == child1);

  g_assert (clutter_actor_get_parent (child1) == actor);
  g_assert_null (clutter_actor_get_next_sibling (child1));
  g_assert_null (clutter_actor_get_previous_sibling (child1));

  child2 = clutter_actor_new ();
  g_object_add_weak_pointer (G_OBJECT (child2), (gpointer *) &child2);

  clutter_actor_add_child (actor, child2);
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, 2);
  g_assert_true (clutter_actor_contains (actor, child2));
  g_assert (clutter_actor_get_first_child (actor) != child2);
  g_assert (clutter_actor_get_last_child (actor) == child2);

  g_assert (clutter_actor_get_parent (child2) == actor);
  g_assert_null (clutter_actor_get_previous_sibling (child1));
  g_assert (clutter_actor_get_next_sibling (child1) == child2);
  g_assert (clutter_actor_get_previous_sibling (child2) == child1);
  g_assert_null (clutter_actor_get_next_sibling (child2));

  g_object_unref (actor);
  g_assert_null (child1);
  g_assert_null (child2);
  g_assert_null (actor);
}

static void
actor_graph_insert_after (void)
{
  ClutterActor *actor, *child, *sibling;

  actor = g_object_ref_sink (clutter_actor_new ());
  clutter_actor_set_name (actor, "root");
  g_object_add_weak_pointer (G_OBJECT (actor), (gpointer *) &actor);

  child = clutter_actor_new ();
  clutter_actor_set_name (child, "child1");
  clutter_actor_insert_child_after (actor, child, NULL);

  if (g_test_verbose ())
    {
      char *s = clutter_test_strdup_graph (actor);
      g_print ("%s\n", s);
      g_free (s);
    }
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, 1);
  g_assert (child == clutter_actor_get_first_child (actor));
  g_assert (child == clutter_actor_get_last_child (actor));
  g_assert_null (clutter_actor_get_next_sibling (child));
  g_assert_null (clutter_actor_get_previous_sibling (child));
  sibling = child;

  child = clutter_actor_new ();
  clutter_actor_set_name (child, "child2");
  clutter_actor_insert_child_after (actor, child, sibling);

  if (g_test_verbose ())
    {
      char *s = clutter_test_strdup_graph (actor);
      g_print ("%s\n", s);
      g_free (s);
    }
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, 2);
  g_assert (child != clutter_actor_get_first_child (actor));
  g_assert (sibling == clutter_actor_get_first_child (actor));
  g_assert (child == clutter_actor_get_last_child (actor));
  g_assert_null (clutter_actor_get_next_sibling (child));
  g_assert (clutter_actor_get_previous_sibling (child) == sibling);

  child = clutter_actor_new ();
  clutter_actor_set_name (child, "child3");
  clutter_actor_insert_child_after (actor, child, sibling);

  if (g_test_verbose ())
    {
      char *s = clutter_test_strdup_graph (actor);
      g_print ("%s\n", s);
      g_free (s);
    }
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, 3);
  g_assert (child != clutter_actor_get_first_child (actor));
  g_assert (child != clutter_actor_get_last_child (actor));

  child = clutter_actor_get_child_at_index (actor, 0);
  g_assert_cmpstr (clutter_actor_get_name (child), ==, "child1");
  g_assert (clutter_actor_get_first_child (actor) == child);
  clutter_actor_set_child_after (actor, child, NULL);

  if (g_test_verbose ())
    {
      char *s = clutter_test_strdup_graph (actor);
      g_print ("%s\n", s);
      g_free (s);
    }
  g_assert (child != clutter_actor_get_first_child (actor));
  g_assert (child == clutter_actor_get_last_child (actor));

  g_object_unref (actor);
  g_assert_null (actor);
}

static void
actor_graph_insert_before (void)
{
  ClutterActor *actor, *child, *sibling;

  actor = g_object_ref_sink (clutter_actor_new ());
  clutter_actor_set_name (actor, "root");
  g_object_add_weak_pointer (G_OBJECT (actor), (gpointer *) &actor);

  child = clutter_actor_new ();
  clutter_actor_set_name (child, "child1");
  clutter_actor_insert_child_before (actor, child, NULL);

  if (g_test_verbose ())
    {
      char *s = clutter_test_strdup_graph (actor);
      g_print ("%s\n", s);
      g_free (s);
    }
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, 1);
  g_assert (child == clutter_actor_get_first_child (actor));
  g_assert (child == clutter_actor_get_last_child (actor));
  g_assert_null (clutter_actor_get_next_sibling (child));
  g_assert_null (clutter_actor_get_previous_sibling (child));
  sibling = child;

  child = clutter_actor_new ();
  clutter_actor_set_name (child, "child2");
  clutter_actor_insert_child_before (actor, child, sibling);

  if (g_test_verbose ())
    {
      char *s = clutter_test_strdup_graph (actor);
      g_print ("%s\n", s);
      g_free (s);
    }
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, 2);
  g_assert (child == clutter_actor_get_first_child (actor));
  g_assert (sibling != clutter_actor_get_first_child (actor));
  g_assert (sibling == clutter_actor_get_last_child (actor));
  g_assert_null (clutter_actor_get_next_sibling (sibling));
  g_assert (clutter_actor_get_next_sibling (child) == sibling);

  child = clutter_actor_new ();
  clutter_actor_set_name (child, "child3");
  clutter_actor_insert_child_before (actor, child, sibling);

  if (g_test_verbose ())
    {
      char *s = clutter_test_strdup_graph (actor);
      g_print ("%s\n", s);
      g_free (s);
    }
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, 3);
  g_assert (child != clutter_actor_get_first_child (actor));
  g_assert (child != clutter_actor_get_last_child (actor));

  child = clutter_actor_get_child_at_index (actor, 2);
  g_assert_cmpstr (clutter_actor_get_name (child), ==, "child1");
  g_assert (clutter_actor_get_last_child (actor) == child);
  clutter_actor_set_child_before (actor, child, NULL);

  if (g_test_verbose ())
    {
      char *s = clutter_test_strdup_graph (actor);
      g_print ("%s\n", s);
      g_free (s);
    }
  g_assert (child == clutter_actor_get_first_child (actor));
  g_assert (child != clutter_actor_get_last_child (actor));

  g_object_unref (actor);
  g_assert_null (actor);
}

static void
actor_graph_insert_at_index (void)
{
  ClutterActor *actor, *child;
  ClutterActorIter iter;
  int i, n_actors;

  actor = g_object_ref_sink (clutter_actor_new ());
  clutter_actor_set_name (actor, "root");
  g_object_add_weak_pointer (G_OBJECT (actor), (gpointer *) &actor);

  n_actors = g_random_int_range (10, 50);

  for (i = 0; i < n_actors; i++)
    {
      ClutterActor *tmp = clutter_actor_new ();
      char *name = g_strdup_printf ("actor-%02d", i);

      clutter_actor_set_name (tmp, name);

      clutter_actor_insert_child_at_index (actor, tmp, i);

      g_free (name);
    }

  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, n_actors);

  i = 0;
  clutter_actor_iter_init (&iter, actor);
  while (clutter_actor_iter_next (&iter, &child))
    {
      char *name = g_strdup_printf ("actor-%02d", i++);

      g_assert_cmpstr (name, ==, clutter_actor_get_name (child));

      g_free (name);
    }

  i = g_random_int_range (0, clutter_actor_get_n_children (actor) - 1);
  child = clutter_actor_get_child_at_index (actor, i);
  g_assert (child != NULL);

  clutter_actor_set_child_at_index (actor, child, -1);
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, n_actors);
  g_assert (clutter_actor_get_last_child (actor) == child);
  g_assert_null (clutter_actor_get_next_sibling (child));
  g_assert (clutter_actor_get_previous_sibling (child) != NULL);

  clutter_actor_set_child_at_index (actor, child, 0);
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, n_actors);
  g_assert (clutter_actor_get_first_child (actor) == child);
  g_assert_null (clutter_actor_get_previous_sibling (child));
  g_assert (clutter_actor_get_next_sibling (child) != NULL);

  clutter_actor_set_child_at_index (actor, child, clutter_actor_get_n_children (actor));
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, n_actors);
  g_assert (clutter_actor_get_last_child (actor) == child);

  g_object_unref (actor);
  g_assert_null (actor);
}

static void
actor_graph_remove_child (void)
{
  ClutterActor *actor, *child;
  int i, n_actors;

  actor = g_object_ref_sink (clutter_actor_new ());
  g_object_add_weak_pointer (G_OBJECT (actor), (gpointer *) &actor);

  n_actors = g_random_int_range (10, 50);

  for (i = 0; i < n_actors; i++)
    {
      ClutterActor *tmp = clutter_actor_new ();
      char *name = g_strdup_printf ("actor-%02d", i);

      clutter_actor_set_name (tmp, name);

      clutter_actor_add_child (actor, tmp);

      g_free (name);
    }

  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, n_actors);

  i = g_random_int_range (0, clutter_actor_get_n_children (actor) - 1);
  child = clutter_actor_get_child_at_index (actor, i);
  g_assert (child != NULL);

  g_object_add_weak_pointer (G_OBJECT (child), (gpointer *) &child);

  clutter_actor_remove_child (actor, child);
  g_assert_cmpint (clutter_actor_get_n_children (actor), <, n_actors);
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, n_actors - 1);
  g_assert_null (child);

  i = g_random_int_range (0, clutter_actor_get_n_children (actor) - 1);
  child = clutter_actor_get_child_at_index (actor, i);
  g_assert (child != NULL);

  g_object_add_weak_pointer (G_OBJECT (child), (gpointer *) &child);

  g_object_ref (child);
  clutter_actor_remove_child (actor, child);
  g_assert_cmpint (clutter_actor_get_n_children (actor), <, n_actors);
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, n_actors - 2);
  g_assert (child != NULL);
  g_assert_null (clutter_actor_get_parent (child));
  g_assert_null (clutter_actor_get_next_sibling (child));
  g_assert_null (clutter_actor_get_previous_sibling (child));

  g_object_unref (actor);
  g_assert_null (actor);
}

static void
actor_graph_remove_all (void)
{
  ClutterActor *actor;
  int i, n_actors;

  actor = g_object_ref_sink (clutter_actor_new ());
  g_object_add_weak_pointer (G_OBJECT (actor), (gpointer *) &actor);

  n_actors = g_random_int_range (10, 50);

  for (i = 0; i < n_actors; i++)
    {
      ClutterActor *tmp = clutter_actor_new ();
      char *name = g_strdup_printf ("actor-%02d", i);

      clutter_actor_set_name (tmp, name);

      clutter_actor_add_child (actor, tmp);

      g_free (name);
    }

  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, n_actors);

  clutter_actor_remove_all_children (actor);
  g_assert_cmpint (clutter_actor_get_n_children (actor), ==, 0);

  g_object_unref (actor);
  g_assert_null (actor);
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_bug_base ("http://bugzilla.gnome.org/");

  g_test_add_func ("/actor/graph/initial-state", actor_graph_initial_state);
  g_test_add_func ("/actor/graph/add-child", actor_graph_add_child);
  g_test_add_func ("/actor/graph/remove-child", actor_graph_remove_child);
  g_test_add_func ("/actor/graph/remove-all", actor_graph_remove_all);
  g_test_add_func ("/actor/graph/insert/after", actor_graph_insert_after);
  g_test_add_func ("/actor/graph/insert/before", actor_graph_insert_before);
  g_test_add_func ("/actor/graph/insert/at-index", actor_graph_insert_at_index);

  return g_test_run ();
}
