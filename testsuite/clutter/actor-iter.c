#include <clutter/clutter.h>

#define ADD_ITER_TEST(path,test_func) \
  g_test_add (path, \
              TestGraph, NULL, \
              test_graph_setup, \
              test_func, \
              test_graph_teardown)

typedef struct {
  ClutterActor *root;

  ClutterActorIter iter;

  int expected_n_children;
} TestGraph;

static void
test_graph_setup (TestGraph *fixture,
                  gconstpointer dummy G_GNUC_UNUSED)
{
  int i;

  fixture->root = clutter_actor_new ();
  g_object_add_weak_pointer (G_OBJECT (fixture->root), (gpointer *) &(fixture->root));

  fixture->expected_n_children = g_random_int_range (10, 50);
  for (i = 0; i < fixture->expected_n_children; i += 1)
    {
      ClutterActor *child;
      char *name;

      name = g_strdup_printf ("actor-%02d", i);

      child = clutter_actor_new ();
      clutter_actor_set_name (child, name);

      clutter_actor_add_child (fixture->root, child);

      g_free (name);
    }

  g_assert_cmpint (clutter_actor_get_n_children (fixture->root), ==, fixture->expected_n_children);

  clutter_actor_iter_init (&fixture->iter, fixture->root);
  g_assert_true (clutter_actor_iter_is_valid (&fixture->iter));
}

static void
test_graph_teardown (TestGraph *fixture,
                     gconstpointer dummy G_GNUC_UNUSED)
{
  g_assert (clutter_actor_iter_is_valid (&fixture->iter));

  g_object_unref (fixture->root);
  g_assert_null (fixture->root);
}

static void
actor_iter_next (TestGraph *fixture,
                 gconstpointer dummy G_GNUC_UNUSED)
{
  ClutterActor *child;
  int i = 0;

  while (clutter_actor_iter_next (&fixture->iter, &child))
    {
      g_assert (CLUTTER_IS_ACTOR (child));
      g_assert (clutter_actor_get_parent (child) == fixture->root);

      if (g_test_verbose ())
        g_print ("actor %d = '%s'\n", i, clutter_actor_get_name (child));

      if (i == 0)
        g_assert (child == clutter_actor_get_first_child (fixture->root));

      if (i == (fixture->expected_n_children - 1))
        g_assert (child == clutter_actor_get_last_child (fixture->root));

      i += 1;
    }

  g_assert_cmpint (i, ==, fixture->expected_n_children);
}

static void
actor_iter_prev (TestGraph *fixture,
                 gconstpointer dummy G_GNUC_UNUSED)
{
  ClutterActor *child;
  int i = 0;

  while (clutter_actor_iter_prev (&fixture->iter, &child))
    {
      g_assert (CLUTTER_IS_ACTOR (child));
      g_assert (clutter_actor_get_parent (child) == fixture->root);

      if (g_test_verbose ())
        g_print ("actor %d = '%s'\n", i, clutter_actor_get_name (child));

      if (i == 0)
        g_assert (child == clutter_actor_get_last_child (fixture->root));

      if (i == (fixture->expected_n_children - 1))
        g_assert (child == clutter_actor_get_first_child (fixture->root));

      i += 1;
    }

  g_assert_cmpint (i, ==, fixture->expected_n_children);
}

static void
actor_iter_remove (TestGraph *fixture,
                   gconstpointer dummy G_GNUC_UNUSED)
{
  ClutterActor *child;
  int i = 0;

  while (clutter_actor_iter_next (&fixture->iter, &child))
    {
      g_assert (CLUTTER_IS_ACTOR (child));
      g_assert (clutter_actor_get_parent (child) == fixture->root);

      if (g_test_verbose ())
        g_print ("actor %d = '%s'\n", i, clutter_actor_get_name (child));

      if (i == 0)
        g_assert (child == clutter_actor_get_first_child (fixture->root));

      if (i == (fixture->expected_n_children - 1))
        g_assert (child == clutter_actor_get_last_child (fixture->root));

      clutter_actor_iter_remove_child (&fixture->iter);
      g_assert (clutter_actor_iter_is_valid (&fixture->iter));

      i += 1;
    }

  g_assert_cmpint (i, ==, fixture->expected_n_children);
  g_assert_cmpint (0, ==, clutter_actor_get_n_children (fixture->root));
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_bug_base ("http://bugzilla.gnome.org/");

  ADD_ITER_TEST ("/actor/iter/next", actor_iter_next);
  ADD_ITER_TEST ("/actor/iter/prev", actor_iter_prev);
  ADD_ITER_TEST ("/actor/iter/remove", actor_iter_remove);

  return g_test_run ();
}
