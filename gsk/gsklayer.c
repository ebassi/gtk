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

/**
 * SECTION:gsklayer
 * @Short_description: Base element in the scene graph
 * @Title: GskLayer
 *
 * #GskLayer is the base element inside a GSK scene graph.
 */

#include "config.h"

#include "gsklayerprivate.h"
#include "gskdebug.h"

/**
 * GskLayer:
 *
 * The `GskLayer` structure contains only private data.
 *
 * Since: 3.18
 */

#define GSK_LAYER_PRIV(obj)     ((GskLayerPrivate *) gsk_layer_get_instance_private ((GskLayer *) (obj)))

typedef struct {
  /* The scene graph */
  GskLayer *parent;
  GskLayer *next_sibling;
  GskLayer *prev_sibling;
  GskLayer *first_child;
  GskLayer *last_child;

  guint n_children;

  char *debug_name;

  /* The age of the layer; used by LayerIter */
  gint64 age;

  /* The target state of the layer */
  GskLayerState model;

  GdkFrameClock *frame_clock;

  /* Bitfields: keep at the end */
  guint hidden : 1;
  guint needs_redraw : 1;
  guint needs_relayout : 1;
  guint in_layout : 1;
} GskLayerPrivate;

enum {
  PROP_PARENT = 1,
  PROP_NEXT_SIBLING,
  PROP_PREV_SIBLING,
  PROP_FIRST_CHILD,
  PROP_LAST_CHILD,
  PROP_N_CHILDREN,

  PROP_HIDDEN,

  PROP_BACKGROUND_COLOR,
  PROP_OPACITY,

  PROP_FRAME,
  PROP_PIVOT_POINT,
  PROP_BOUNDS,
  PROP_POSITION,

  PROP_TRANSFORM,
  PROP_CHILD_TRANSFORM,

  N_PROPS
};

enum {
  CHILD_ADDED,
  CHILD_REMOVED,
  LAYOUT_CHILDREN,
  QUEUE_RELAYOUT,
  QUEUE_REDRAW,

  LAST_SIGNAL
};

static void     gsk_layer_queue_redraw_internal         (GskLayer *self,
                                                         GskLayer *origin);
static void     gsk_layer_queue_relayout_internal       (GskLayer *self,
                                                         GskLayer *origin);
static void     gsk_layer_revalidate_layout             (GskLayer *self);

G_DEFINE_TYPE_WITH_PRIVATE (GskLayer, gsk_layer, G_TYPE_INITIALLY_UNOWNED)

static guint layer_signals[LAST_SIGNAL] = { 0, };
static GParamSpec *layer_props[N_PROPS] = { NULL, };

static inline void
gsk_layer_set_parent (GskLayer *self,
                      GskLayer *parent)
{
  g_assert (self != parent);
  GSK_LAYER_PRIV (self)->parent = parent;
}

static inline void
gsk_layer_set_next_sibling (GskLayer *self,
                            GskLayer *sibling)
{
  if (sibling != NULL)
    g_assert (GSK_LAYER_PRIV (sibling)->parent == GSK_LAYER_PRIV (self)->parent);
  GSK_LAYER_PRIV (self)->next_sibling = sibling;
}

static inline void
gsk_layer_set_prev_sibling (GskLayer *self,
                            GskLayer *sibling)
{
  if (sibling != NULL)
    g_assert (GSK_LAYER_PRIV (sibling)->parent == GSK_LAYER_PRIV (self)->parent);
  GSK_LAYER_PRIV (self)->prev_sibling = sibling;
}

static inline void
gsk_layer_set_first_child (GskLayer *self,
                           GskLayer *child)
{
  if (child != NULL)
    g_assert (GSK_LAYER_PRIV (child)->parent == self);
  GSK_LAYER_PRIV (self)->first_child = child;
}

static inline void
gsk_layer_set_last_child (GskLayer *self,
                          GskLayer *child)
{
  if (child != NULL)
    g_assert (GSK_LAYER_PRIV (child)->parent == self);
  GSK_LAYER_PRIV (self)->last_child = child;
}

static inline gboolean
gsk_layer_in_relayout (GskLayer *self)
{
  return GSK_LAYER_PRIV (self)->in_layout;
}

GskLayerState *
gsk_layer_get_state (GskLayer *self)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private (self);

  return &priv->model;
}

static gboolean
gsk_layer_real_queue_redraw (GskLayer *self,
                             GskLayer *origin)
{
  return TRUE;
}

static gboolean
gsk_layer_real_queue_relayout (GskLayer *self,
                               GskLayer *origin)
{
  return TRUE;
}

static void
gsk_layer_real_get_preferred_size (GskLayer        *self,
                                   graphene_size_t *size)
{
  const GeometryInfo *info;

  info = gsk_layer_state_peek_geometry_info (gsk_layer_get_state (self));

  graphene_size_init_from_size (size, &info->bounds.size);
}

static gboolean
gsk_layer_real_draw (GskLayer *self,
                     cairo_t *cr)
{
  GskLayerState *state = gsk_layer_get_state (self);
  const RenderInfo *rinfo = gsk_layer_state_peek_render_info (state);
  const GeometryInfo *ginfo = gsk_layer_state_peek_geometry_info (state);

  cairo_save (cr);

  gdk_cairo_set_source_rgba (cr, &rinfo->background_color);

  cairo_rectangle (cr,
                   ginfo->bounds.origin.x,
                   ginfo->bounds.origin.y,
                   ginfo->bounds.size.width,
                   ginfo->bounds.size.height);
  cairo_fill (cr);

  cairo_restore (cr);

  GSK_LAYER_PRIV (self)->needs_redraw = FALSE;

  return FALSE;
}

static void
gsk_layer_real_layout_children (GskLayer *layer)
{
}

static void
gsk_layer_set_property (GObject      *gobject,
                        guint         prop_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
}

static void
gsk_layer_get_property (GObject    *gobject,
                        guint       prop_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
}

static void
gsk_layer_finalize (GObject *gobject)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private ((GskLayer *) gobject);

  g_clear_pointer (&priv->debug_name, g_free);

  gsk_layer_state_clear (&priv->model);

  G_OBJECT_CLASS (gsk_layer_parent_class)->finalize (gobject);
}

static void
gsk_layer_dispose (GObject *gobject)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private ((GskLayer *) gobject);

  if (priv->frame_clock != NULL)
    g_signal_handlers_disconnect_by_func (priv->frame_clock,
                                          gsk_layer_revalidate_layout,
                                          gobject);

  g_clear_object (&priv->frame_clock);

  G_OBJECT_CLASS (gsk_layer_parent_class)->dispose (gobject);
}

static void
gsk_layer_class_init (GskLayerClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gsk_layer_set_property;
  gobject_class->get_property = gsk_layer_get_property;
  gobject_class->dispose = gsk_layer_dispose;
  gobject_class->finalize = gsk_layer_finalize;

  klass->queue_redraw = gsk_layer_real_queue_redraw;
  klass->queue_relayout = gsk_layer_real_queue_relayout;
  klass->get_preferred_size = gsk_layer_real_get_preferred_size;
  klass->draw = gsk_layer_real_draw;
  klass->layout_children = gsk_layer_real_layout_children;

  layer_signals[CHILD_ADDED] =
    g_signal_new (g_intern_static_string ("child-added"),
                  GSK_TYPE_LAYER,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GskLayerClass, child_added),
                  NULL, NULL,
                  NULL,
                  G_TYPE_NONE, 1, GSK_TYPE_LAYER);

  layer_signals[CHILD_REMOVED] =
    g_signal_new (g_intern_static_string ("child-removed"),
                  GSK_TYPE_LAYER,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GskLayerClass, child_removed),
                  NULL, NULL,
                  NULL,
                  G_TYPE_NONE, 1, GSK_TYPE_LAYER);

  layer_signals[LAYOUT_CHILDREN] =
    g_signal_new (g_intern_static_string ("layout-children"),
                  GSK_TYPE_LAYER,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GskLayerClass, layout_children),
                  NULL, NULL,
                  NULL,
                  G_TYPE_NONE, 0);

  layer_signals[QUEUE_RELAYOUT] =
    g_signal_new (g_intern_static_string ("queue-relayout"),
                  GSK_TYPE_LAYER,
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  NULL,
                  G_TYPE_NONE, 0);

  layer_signals[QUEUE_REDRAW] =
    g_signal_new (g_intern_static_string ("queue-redraw"),
                  GSK_TYPE_LAYER,
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  NULL,
                  G_TYPE_NONE, 0);
}

static void
gsk_layer_init (GskLayer *self)
{
}

/**
 * gsk_layer_new:
 *
 * Creates a new #GskLayer instance.
 *
 * Returns: (transfer full): the newly created #GskLayer instance
 *
 * Since: 3.18
 */
GskLayer *
gsk_layer_new (void)
{
  return g_object_new (GSK_TYPE_LAYER, NULL);
}

#define GSK_LAYER_DEFINE_GETTER(TypeName, type_name, FieldType, field_name) \
FieldType \
type_name ## _get_ ## field_name (TypeName *self) \
{ \
  TypeName ## Private *priv = type_name ## _get_instance_private (self); \
\
  g_return_val_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (self, type_name ## _get_type ()), (FieldType) 0); \
\
  return priv->field_name; \
}

/**
 * gsk_layer_get_parent:
 * @self: a #GskLayer
 *
 * Retrieves the parent layer of a #GskLayer.
 *
 * Returns: (transfer none) (nullable): a #GskLayer
 *
 * Since: 3.18
 */
GSK_LAYER_DEFINE_GETTER (GskLayer, gsk_layer, GskLayer *, parent)

/**
 * gsk_layer_get_next_sibling:
 * @self: a #GskLayer
 *
 * Retrieves the next sibling of a #GskLayer.
 *
 * Returns: (transfer none) (nullable): a #GskLayer
 *
 * Since: 3.18
 */
GSK_LAYER_DEFINE_GETTER (GskLayer, gsk_layer, GskLayer *, next_sibling)

/**
 * gsk_layer_get_prev_sibling:
 * @self: a #GskLayer
 *
 * Retrieves the previous sibling of a #GskLayer.
 *
 * Returns: (transfer none) (nullable): a #GskLayer
 *
 * Since: 3.18
 */
GSK_LAYER_DEFINE_GETTER (GskLayer, gsk_layer, GskLayer *, prev_sibling)

/**
 * gsk_layer_get_first_child:
 * @self: a #GskLayer
 *
 * Retrieves the first child of a #GskLayer.
 *
 * Returns: (transfer none) (nullable): a #GskLayer
 *
 * Since: 3.18
 */
GSK_LAYER_DEFINE_GETTER (GskLayer, gsk_layer, GskLayer *, first_child)

/**
 * gsk_layer_get_last_child:
 * @self: a #GskLayer
 *
 * Retrieves the last child of a #GskLayer.
 *
 * Returns: (transfer none) (nullable): a #GskLayer
 *
 * Since: 3.18
 */
GSK_LAYER_DEFINE_GETTER (GskLayer, gsk_layer, GskLayer *, last_child)

/**
 * gsk_layer_get_n_children:
 * @self: a #GskLayer
 *
 * Retrieves the number of children of a #GskLayer
 *
 * Returns: the number of children of the layer
 *
 * Since: 3.18
 */
GSK_LAYER_DEFINE_GETTER (GskLayer, gsk_layer, guint, n_children)

#undef GSK_LAYER_DEFINE_GETTER

/*< private >
 * gsk_layer_get_age:
 * @self: a #GskLayer
 *
 * Retrieves the age of the layer. The age changes when the
 * list of children of the layer changes.
 *
 * Returns: the age of the layer
 */
gint64
gsk_layer_get_age (GskLayer *self)
{
  return GSK_LAYER_PRIV (self)->age;
}

/**
 * gsk_layer_set_hidden:
 * @self: a #GskLayer
 * @hidden: whether the layer should be hidden
 *
 * Sets whether a #GskLayer should be hidden.
 *
 * Hidden layers are not drawn, which means that any descendant of
 * a hidden layer is also not drawn.
 *
 * Since: 3.18
 */
void
gsk_layer_set_hidden (GskLayer *self,
                      gboolean  hidden)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private (self);

  g_return_if_fail (GSK_IS_LAYER (self));

  hidden = !!hidden;

  if (priv->hidden == hidden)
    return;

  priv->hidden = hidden;

  /* Queue a redraw of the parent, if any is set; we don't want
   * to blow away the cached state of the layer, so we don't
   * queue a redraw on it directly.
   */
  if (priv->parent != NULL)
    gsk_layer_queue_redraw_internal (priv->parent, self);
}

/**
 * gsk_layer_get_hidden:
 * @self: a #GskLayer
 *
 * Checks whether a #GskLayer has been hidden.
 *
 * Returns: %TRUE if the layer was hidden
 *
 * Since: 3.18
 */
gboolean
gsk_layer_get_hidden (GskLayer *self)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private (self);

  g_return_val_if_fail (GSK_IS_LAYER (self), FALSE);

  return priv->hidden;
}

gboolean
gsk_layer_draw (GskLayer *self,
                cairo_t  *cr)
{
  return GSK_LAYER_GET_CLASS (self)->draw (self, cr);
}

/*< private >
 * gsk_layer_get_debug_name:
 * @self: a #GskLayer
 *
 * Retrieves a printable string representing the layer, to
 * be used in debug messages.
 *
 * Returns: (transfer none): the printable debug name of the layer
 */
const char *
gsk_layer_get_debug_name (GskLayer *self)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private (self);

  if (G_UNLIKELY (priv->debug_name == NULL))
    priv->debug_name = g_strdup_printf ("<%s>[%p]", G_OBJECT_TYPE_NAME (self), self);

  return priv->debug_name;
}

static inline void
gsk_layer_remove_child_internal (GskLayer *self,
                                 GskLayer *child)
{
  GskLayer *prev_sibling, *next_sibling;
  GskLayer *old_first, *old_last;

  if (GSK_LAYER_PRIV (child)->parent != self)
    {
      g_critical ("The layer '%s' is not a child of layer '%s', "
                  "so it cannot be removed from its children.",
                  gsk_layer_get_debug_name (child),
                  gsk_layer_get_debug_name (self));
      return;
    }

  old_first = GSK_LAYER_PRIV (self)->first_child;
  old_last = GSK_LAYER_PRIV (self)->last_child;

  prev_sibling = GSK_LAYER_PRIV (child)->prev_sibling;
  next_sibling = GSK_LAYER_PRIV (child)->next_sibling;

  if (prev_sibling != NULL)
    gsk_layer_set_next_sibling (prev_sibling, next_sibling);
  if (next_sibling != NULL)
    gsk_layer_set_prev_sibling (next_sibling, prev_sibling);

  if (GSK_LAYER_PRIV (self)->first_child == child)
    gsk_layer_set_first_child (self, next_sibling);
  if (GSK_LAYER_PRIV (self)->last_child == child)
    gsk_layer_set_last_child (self, prev_sibling);

  GSK_LAYER_PRIV (self)->n_children -= 1;
  GSK_LAYER_PRIV (self)->age += 1;

  /* Unset the graph fields on the child */
  gsk_layer_set_parent (child, NULL);
  gsk_layer_set_prev_sibling (child, NULL);
  gsk_layer_set_next_sibling (child, NULL);

  g_signal_emit (self, layer_signals[CHILD_REMOVED], 0, child);

  /* Release the reference acquired in gsk_layer_add_child_internal() */
  g_object_unref (child);

  if (old_first != GSK_LAYER_PRIV (self)->first_child)
    ;

  if (old_last != GSK_LAYER_PRIV (self)->last_child)
    ;
}

typedef void (* GskLayerAddChildFunc) (GskLayer *self,
                                       GskLayer *child,
                                       gpointer  data);

static inline void
gsk_layer_add_child_internal (GskLayer             *self,
                              GskLayer             *child,
                              GskLayerAddChildFunc  add_func,
                              gpointer              add_func_data)
{
  GskLayer *old_first, *old_last;

  if (GSK_LAYER_PRIV (child)->parent != NULL)
    {
      g_critical ("The layer '%s' cannot be a child of layer '%s', "
                  "because it already has a parent layer '%s'.",
                  gsk_layer_get_debug_name (child),
                  gsk_layer_get_debug_name (self),
                  gsk_layer_get_debug_name (GSK_LAYER_PRIV (child)->parent));
      return;
    }

  old_first = GSK_LAYER_PRIV (self)->first_child;
  old_last = GSK_LAYER_PRIV (self)->last_child;

  /* Acquire a reference, to be released when the child is removed */
  g_object_ref_sink (child);

  /* Reset the state of the child */
  gsk_layer_set_parent (child, self);
  gsk_layer_set_prev_sibling (child, NULL);
  gsk_layer_set_next_sibling (child, NULL);

  /* Call the delegate function for setting up the graph */
  add_func (self, child, add_func_data);

  GSK_LAYER_PRIV (self)->n_children += 1;
  GSK_LAYER_PRIV (self)->age += 1;

  g_signal_emit (self, layer_signals[CHILD_ADDED], 0, child);

  /* Relayout the graph */
  gsk_layer_queue_relayout (child);

  if (old_first != GSK_LAYER_PRIV (self)->first_child)
    ;

  if (old_last != GSK_LAYER_PRIV (self)->last_child)
    ;
}

static void
insert_child_at_index (GskLayer *self,
                       GskLayer *child,
                       gpointer  data)
{
  int index_ = GPOINTER_TO_INT (data);

  gsk_layer_set_parent (child, self);

  if (index_ == 0)
    {
      GskLayer *next = GSK_LAYER_PRIV (self)->first_child;

      /* prepend */
      gsk_layer_set_prev_sibling (child, NULL);
      gsk_layer_set_next_sibling (child, next);
      if (next != NULL)
        gsk_layer_set_prev_sibling (next, child);
    }
  else if (index_ < 0 || index_ >= GSK_LAYER_PRIV (self)->n_children)
    {
      GskLayer *prev = GSK_LAYER_PRIV (self)->last_child;

      /* append */
      gsk_layer_set_next_sibling (child, NULL);
      gsk_layer_set_prev_sibling (child, prev);
      if (prev != NULL)
        gsk_layer_set_next_sibling (prev, child);
    }
  else
    {
      GskLayer *iter;
      guint i;

      /* insert */
      for (iter = GSK_LAYER_PRIV (self)->first_child, i = 0;
           iter != NULL;
           iter = GSK_LAYER_PRIV (iter)->next_sibling, i++)
        {
          if (i == index_)
            {
              GskLayer *prev = GSK_LAYER_PRIV (iter)->prev_sibling;
              GskLayer *next = GSK_LAYER_PRIV (iter)->next_sibling;

              gsk_layer_set_prev_sibling (child, prev);
              gsk_layer_set_next_sibling (child, next);

              gsk_layer_set_next_sibling (prev, child);
              gsk_layer_set_prev_sibling (next, child);

              break;
            }
        }
    }

  if (GSK_LAYER_PRIV (child)->prev_sibling == NULL)
    gsk_layer_set_first_child (self, child);
  if (GSK_LAYER_PRIV (child)->next_sibling == NULL)
    gsk_layer_set_last_child (self, child);
}

static void
insert_child_after (GskLayer *self,
                    GskLayer *child,
                    gpointer  data)
{
  GskLayer *sibling = data;

  if (sibling == NULL)
    sibling = GSK_LAYER_PRIV (self)->last_child;

  gsk_layer_set_prev_sibling (child, sibling);

  if (sibling != NULL)
    {
      GskLayer *tmp = GSK_LAYER_PRIV (sibling)->next_sibling;

      gsk_layer_set_next_sibling (child, tmp);

      if (tmp != NULL)
        gsk_layer_set_prev_sibling (tmp, child);

      gsk_layer_set_next_sibling (sibling, child);
    }
  else
    gsk_layer_set_next_sibling (child, NULL);

  if (GSK_LAYER_PRIV (child)->prev_sibling == NULL)
    gsk_layer_set_first_child (self, child);
  if (GSK_LAYER_PRIV (child)->next_sibling == NULL)
    gsk_layer_set_last_child (self, child);
}

static void
insert_child_before (GskLayer *self,
                     GskLayer *child,
                     gpointer  data)
{
  GskLayer *sibling = data;

  if (sibling == NULL)
    sibling = GSK_LAYER_PRIV (self)->first_child;

  gsk_layer_set_next_sibling (child, sibling);

  if (sibling != NULL)
    {
      GskLayer *tmp = GSK_LAYER_PRIV (sibling)->prev_sibling;

      gsk_layer_set_prev_sibling (child, tmp);

      if (tmp != NULL)
        gsk_layer_set_next_sibling (tmp, child);

      gsk_layer_set_prev_sibling (sibling, child);
    }
  else
    gsk_layer_set_prev_sibling (child, NULL);

  if (GSK_LAYER_PRIV (child)->prev_sibling == NULL)
    gsk_layer_set_first_child (self, child);
  if (GSK_LAYER_PRIV (child)->next_sibling == NULL)
    gsk_layer_set_last_child (self, child);
}

typedef struct {
  GskLayer *prev;
  GskLayer *next;
} InsertBetweenData;

static void
insert_child_between (GskLayer *self,
                      GskLayer *child,
                      gpointer  data_)
{
  InsertBetweenData *data = data_;

  gsk_layer_set_prev_sibling (child, data->prev);
  gsk_layer_set_next_sibling (child, data->next);

  if (data->prev != NULL)
    gsk_layer_set_next_sibling (data->prev, child);
  if (data->next != NULL)
    gsk_layer_set_prev_sibling (data->next, child);

  if (GSK_LAYER_PRIV (child)->prev_sibling == NULL)
    gsk_layer_set_first_child (self, child);
  if (GSK_LAYER_PRIV (child)->next_sibling == NULL)
    gsk_layer_set_last_child (self, child);
}

/**
 * gsk_layer_insert_child_at_index:
 * @self: a #GskLayer
 * @child: the child #GskLayer to add
 * @index_: the index of the child layer, starting from zero
 *
 * Inserts a new @child #GskLayer in the list of children of @self,
 * at the given @index_.
 *
 * If @index_ is negative, or greater or equal to the number of children
 * of @self, the @child is appended to the list of children.
 *
 * If @index_ is zero, the @child is prepended to the list of children.
 *
 * Returns: (transfer none): the layer with a new child
 *
 * Since: 3.18
 */
GskLayer *
gsk_layer_insert_child_at_index (GskLayer *self,
                                 GskLayer *child,
                                 int       index_)
{
  g_return_val_if_fail (GSK_IS_LAYER (self), NULL);
  g_return_val_if_fail (GSK_IS_LAYER (child), self);

  gsk_layer_add_child_internal (self, child,
                                insert_child_at_index,
                                GINT_TO_POINTER (index_));

  return self;
}

/**
 * gsk_layer_insert_child_before:
 * @self: a #GskLayer
 * @child: the child layer to add
 * @sibling: (nullable): an optional #GskLayer, child of @self
 *
 * Inserts a new @child #GskLayer in the list of children of @self.
 *
 * If @sibling is set, the @child will be added before @sibling in
 * the list; if it is unset, the @child will be prepended to the
 * list of children.
 *
 * Returns: (transfer none): the layer with a new child
 *
 * Since: 3.18
 */
GskLayer *
gsk_layer_insert_child_before (GskLayer *self,
                               GskLayer *child,
                               GskLayer *sibling)
{
  g_return_val_if_fail (GSK_IS_LAYER (self), NULL);
  g_return_val_if_fail (GSK_IS_LAYER (child), self);
  g_return_val_if_fail (sibling == NULL || GSK_IS_LAYER (sibling), self);

  gsk_layer_add_child_internal (self, child, insert_child_before, sibling);

  return self;
}

/**
 * gsk_layer_insert_child_after:
 * @self: a #GskLayer
 * @child: the child layer to add
 * @sibling: (nullable): an optional #GskLayer, child of @self
 *
 * Inserts a new @child #GskLayer in the list of children of @self.
 *
 * If @sibling is set, the @child will be added after @sibling in
 * the list; if it is unset, the @child will be appended to the
 * list of children.
 *
 * Returns: (transfer none): the layer with a new child
 *
 * Since: 3.18
 */
GskLayer *
gsk_layer_insert_child_after (GskLayer *self,
                              GskLayer *child,
                              GskLayer *sibling)
{
  g_return_val_if_fail (GSK_IS_LAYER (self), NULL);
  g_return_val_if_fail (GSK_IS_LAYER (child), self);
  g_return_val_if_fail (sibling == NULL || GSK_IS_LAYER (sibling), self);

  gsk_layer_add_child_internal (self, child, insert_child_after, sibling);

  return self;
}

/**
 * gsk_layer_replace_child:
 * @self: a #GskLayer
 * @old_child: the child layer to be replaced
 * @new_child: the child layer to replace @old_child
 *
 * Replaces @old_child with @new_child in the list of children of @self.
 *
 * Returns: (transfer none): the layer with the new child
 *
 * Since: 3.18
 */
GskLayer *
gsk_layer_replace_child (GskLayer *self,
                         GskLayer *old_child,
                         GskLayer *new_child)
{
  InsertBetweenData data;

  g_return_val_if_fail (GSK_IS_LAYER (self), NULL);
  g_return_val_if_fail (GSK_IS_LAYER (old_child), self);
  g_return_val_if_fail (GSK_IS_LAYER (new_child), self);

  if (GSK_LAYER_PRIV (old_child)->parent != self)
    {
      g_critical ("Unable to replace layer '%s' in layer '%s': "
                  "it is a child of layer '%s'",
                  gsk_layer_get_debug_name (old_child),
                  gsk_layer_get_debug_name (self),
                  gsk_layer_get_debug_name (GSK_LAYER_PRIV (old_child)->parent));
      return self;
    }

  data.prev = GSK_LAYER_PRIV (old_child)->prev_sibling;
  data.next = GSK_LAYER_PRIV (old_child)->next_sibling;

  gsk_layer_remove_child_internal (self, old_child);
  gsk_layer_add_child_internal (self, new_child, insert_child_between, &data);

  return self;
}

/**
 * gsk_layer_add_child:
 * @self: a #GskLayer
 * @child: the child #GskLayer to add
 *
 * Adds a @child #GskLayer to the list of children of the given #GskLayer.
 *
 * The child is appended to the list of children of @self.
 *
 * The #GskLayer will acquire a reference on the newly added @child.
 *
 * Returns: (transfer none): the layer with a new child
 *
 * Since: 3.18
 */
GskLayer *
gsk_layer_add_child (GskLayer *self,
                     GskLayer *child)
{
  g_return_val_if_fail (GSK_IS_LAYER (self), NULL);
  g_return_val_if_fail (GSK_IS_LAYER (child), self);

  gsk_layer_add_child_internal (self, child, insert_child_after, NULL);

  return self;
}

/**
 * gsk_layer_remove_child:
 * @self: a #GskLayer
 * @child: the child #GskLayer to remove
 *
 * Removes a @child #GskLayer from the list of children of the given #GskLayer.
 *
 * The #GskLayer will release the reference acquired on the @child.
 *
 * Returns: (transfer none): the layer without the child
 *
 * Since: 3.18
 */
GskLayer *
gsk_layer_remove_child (GskLayer *self,
                        GskLayer *child)
{
  g_return_val_if_fail (GSK_IS_LAYER (self), NULL);
  g_return_val_if_fail (GSK_IS_LAYER (child), self);

  gsk_layer_remove_child_internal (self, child);

  return self;
}

/**
 * gsk_layer_get_child_at_index:
 * @self: a #GskLayer
 * @index_: the index of the child to retrieve
 *
 * Retrieves the #GskLayer in the list of children of @self, at the
 * given index.
 *
 * Returns: (transfer none): the layer at the given index
 *
 * Since: 3.18
 */
GskLayer *
gsk_layer_get_child_at_index (GskLayer *self,
                              int       index_)
{
  GskLayer *iter;
  int i;

  g_return_val_if_fail (GSK_IS_LAYER (self), NULL);
  g_return_val_if_fail (index_ <= GSK_LAYER_PRIV (self)->n_children, NULL);

  for (iter = GSK_LAYER_PRIV (self)->first_child, i = 0;
       iter != NULL;
       iter = GSK_LAYER_PRIV (iter)->next_sibling, i++)
    {
      if (i == index_)
        return iter;
    }

  return NULL;
}

/**
 * gsk_layer_contains:
 * @self: a #GskLayer
 * @descendant: the descendant to check
 *
 * Checks whether @self contains @descendant.
 *
 * Returns: %TRUE if @self contains the given #GskLayer
 *
 * Since: 3.18
 */
gboolean
gsk_layer_contains (GskLayer *self,
                    GskLayer *descendant)
{
  GskLayer *iter;

  g_return_val_if_fail (GSK_IS_LAYER (self), FALSE);
  g_return_val_if_fail (GSK_IS_LAYER (descendant), FALSE);

  for (iter = descendant; iter != NULL; iter = GSK_LAYER_PRIV (iter)->parent)
    {
      if (iter == self)
        return TRUE;
    }

  return FALSE;
}

static GskLayer *
gsk_layer_get_top_level (GskLayer *self)
{
  GskLayer *iter;

  iter = self;
  while (iter != NULL)
    {
      if (GSK_LAYER_PRIV (iter)->parent == NULL)
        return iter;

      iter = GSK_LAYER_PRIV (iter)->parent;
    }

  return NULL;
}

static void
gsk_layer_queue_redraw_internal (GskLayer *self,
                                 GskLayer *origin)
{
  GskLayer *iter;

  iter = self;
  while (iter != NULL)
    {
      GskLayerPrivate *priv = gsk_layer_get_instance_private (iter);

      /* We reached a point in the graph where we already queued a
       * redraw, so we don't need to recurse any further
       */
      if (priv->needs_redraw)
        break;

      /* We ask the layer's implementation whether or not the layer content
       * should be invalidated; this allows layers to bail out of the invalidation
       * process.
       */
      priv->needs_redraw = GSK_LAYER_GET_CLASS (iter)->queue_redraw (iter, origin);
      if (!priv->needs_redraw)
        break;

      iter = priv->parent;
    }

  iter = gsk_layer_get_top_level (self);
  if (GSK_LAYER_PRIV (iter)->frame_clock != NULL)
    g_signal_emit (iter, layer_signals[QUEUE_REDRAW], 0);
}

static void
gsk_layer_queue_relayout_internal (GskLayer *self,
                                   GskLayer *origin)
{
  GskLayer *iter;

  iter = self;
  while (iter != NULL)
    {
      GskLayerPrivate *priv = gsk_layer_get_instance_private (iter);

      /* Hidden branches do not need to propagate further */
      if (priv->hidden)
        break;

      /* We reached a point in the graph where we already queued a
       * relayout, so we don't need to recurse any further
       */
      if (priv->needs_relayout)
        break;

      /* We ask the layer's implementation whether or not the layer geometry
       * should be invalidated; this allows layers to bail out of the invalidation
       * process.
       */
      priv->needs_relayout = GSK_LAYER_GET_CLASS (iter)->queue_relayout (iter, origin);
      if (!priv->needs_relayout)
        break;

      iter = priv->parent;
    }

  if (iter == NULL)
    {
      iter = gsk_layer_get_top_level (self);
      if (GSK_LAYER_PRIV (iter)->frame_clock != NULL)
        g_signal_emit (iter, layer_signals[QUEUE_RELAYOUT], 0);
    }
}

static inline void
gsk_layer_maybe_queue_relayout (GskLayer *self)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private (self);

  if (priv->parent == NULL && !priv->needs_relayout)
    {
      priv->needs_relayout = TRUE;

      if (priv->frame_clock != NULL)
        {
          GSK_NOTE (LAYOUT, g_print (G_STRLOC ": called on root, requesting layout."));
          gdk_frame_clock_request_phase (priv->frame_clock, GDK_FRAME_CLOCK_PHASE_LAYOUT);
          return;
        }
    }

  if (priv->parent != NULL)
    {
      if (priv->needs_relayout && gsk_layer_in_relayout (priv->parent))
        {
          GSK_NOTE (LAYOUT, g_print (G_STRLOC ": called during parent's layout."));
          priv->needs_relayout = FALSE;
          return;
        }
    }

  gsk_layer_queue_relayout (self);
}

/**
 * gsk_layer_queue_redraw:
 * @self: a #GskLayer
 *
 * Queues a redraw on a #GskLayer.
 *
 * This function should be called when the contents of the
 * layer change.
 *
 * See also: gsk_layer_queue_relayout()
 *
 * Since: 3.18
 */
void
gsk_layer_queue_redraw (GskLayer *self)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private (self);

  g_return_if_fail (GSK_IS_LAYER (self));

  /* Hidden layers do no contribute to the content */
  if (priv->hidden)
    return;

  gsk_layer_queue_redraw_internal (self, self);
}

/**
 * gsk_layer_needs_redraw:
 * @self: a #GskLayer
 *
 * Checks whether the content of a #GskLayer has been invalidated,
 * and the layer should be redrawn.
 *
 * See also: gsk_layer_needs_relayout()
 *
 * Returns: %TRUE if the #GskLayer needs to be redrawn
 *
 * Since: 3.18
 */
gboolean
gsk_layer_needs_redraw (GskLayer *self)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private (self);

  g_return_val_if_fail (GSK_IS_LAYER (self), FALSE);

  return priv->needs_redraw;
}

/**
 * gsk_layer_queue_relayout:
 * @self: a #GskLayer
 *
 * Queues a relayout on a #GskLayer.
 *
 * This function should be called when the geometry of the
 * layer change.
 *
 * See also: gsk_layer_queue_redraw()
 *
 * Since: 3.18
 */
void
gsk_layer_queue_relayout (GskLayer *self)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private (self);

  g_return_if_fail (GSK_IS_LAYER (self));

  /* Hidden layers do not contribute to the layout */
  if (priv->hidden)
    return;

  gsk_layer_queue_relayout_internal (self, self);
}

/**
 * gsk_layer_needs_relayout:
 * @self: a #GskLayer
 *
 * Checks whether the geometry of a #GskLayer has been invalidated,
 * and the layer should be resized.
 *
 * Returns: %TRUE if the #GskLayer needs to be resized
 *
 * Since: 3.18
 */
gboolean
gsk_layer_needs_relayout (GskLayer *self)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private (self);

  g_return_val_if_fail (GSK_IS_LAYER (self), FALSE);

  return priv->needs_relayout;
}

/**
 * gsk_layer_set_pivot_point:
 * @self: a #GskLayer
 * @point: the coordinates of the pivot point
 *
 * Sets the coordinates of the pivot point of a #GskLayer.
 *
 * The pivot point is used to determine the #GskLayer:frame of the layer,
 * as well as the center of all the transformations.
 *
 * The coordinates of the point are in layer-relative, normalized space,
 * which means that the (0, 0) coordinates are the top-left corner of the
 * layer; the (0.5, 0.5) coordinates are the center of the layer; and the
 * (1, 1) coordinates are the bottom-right corner of the layer.
 *
 * Since: 3.18
 */
void
gsk_layer_set_pivot_point (GskLayer               *self,
                           const graphene_point_t *point)
{
  GeometryInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (point != NULL);

  info = gsk_layer_state_get_geometry_info (gsk_layer_get_state (self));
  graphene_point_init_from_point (&info->pivot_point, point);
  gsk_layer_maybe_queue_relayout (self);
}

/**
 * gsk_layer_get_pivot_point:
 * @self: a #GskLayer
 * @point: (out caller-allocates): return location for the pivot point
 *
 * Retrieves the coordinates of the pivot point of a #GskLayer.
 *
 * Since: 3.18
 */
void
gsk_layer_get_pivot_point (GskLayer *self,
                           graphene_point_t *point)
{
  const GeometryInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (point != NULL);

  info = gsk_layer_state_peek_geometry_info (gsk_layer_get_state (self));
  graphene_point_init_from_point (point, &info->pivot_point);
}

/**
 * gsk_layer_set_position:
 * @self: a #GskLayer
 * @point: the coordinates of the position
 *
 * Sets the coordinates of the position of a #GskLayer.
 *
 * The coordinates of the position are in parent-relative space,
 * and specify the origin of the #GskLayer:frame of the layer.
 *
 * Since: 3.18
 */
void
gsk_layer_set_position (GskLayer               *self,
                        const graphene_point_t *point)
{
  GeometryInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (point != NULL);

  info = gsk_layer_state_get_geometry_info (gsk_layer_get_state (self));
  graphene_point_init_from_point (&info->position, point);

  gsk_layer_maybe_queue_relayout (self);
}

/**
 * gsk_layer_get_position:
 * @self: a #GskLayer
 * @position: (out caller-allocates): return location for the position
 *
 * Retrieves the coordinates of the position of a #GskLayer.
 *
 * Since: 3.18
 */
void
gsk_layer_get_position (GskLayer *self,
                        graphene_point_t *position)
{
  const GeometryInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (position != NULL);

  info = gsk_layer_state_peek_geometry_info (gsk_layer_get_state (self));
  graphene_point_init_from_point (position, &info->position);
}

/**
 * gsk_layer_set_bounds:
 * @self: a #GskLayer
 * @bounds: the bounds rectangle
 *
 * Sets the bounds rectangle of a #GskLayer.
 *
 * The bounds rectangle has an origin in the top-left corner of
 * the layer, and it's expressed in layer-relative coordinate space.
 *
 * Since: 3.18
 */
void
gsk_layer_set_bounds (GskLayer              *self,
                      const graphene_rect_t *bounds)
{
  GeometryInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (bounds != NULL);

  info = gsk_layer_state_get_geometry_info (gsk_layer_get_state (self));
  graphene_rect_init_from_rect (&info->bounds, bounds);
  gsk_layer_maybe_queue_relayout (self);
}

/**
 * gsk_layer_get_bounds:
 * @self: a #GskLayer
 * @bounds: (out caller-allocates): return location for the bounds
 *   rectangle
 *
 * Retrieves the bounds rectangle of a #GskLayer.
 *
 * The bounds rectangle has an origin expressed in layer-relative coordinate
 * space; if you need the parent-relative coordinates, use gsk_layer_get_frame()
 * instead.
 *
 * Since: 3.18
 */
void
gsk_layer_get_bounds (GskLayer        *self,
                      graphene_rect_t *bounds)
{
  const GeometryInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (bounds != NULL);

  info = gsk_layer_state_peek_geometry_info (gsk_layer_get_state (self));
  graphene_rect_init_from_rect (bounds, &info->bounds);
}

/**
 * gsk_layer_set_frame:
 * @self: a #GskLayer
 * @frame: the frame rectangle
 *
 * Sets the frame rectangle of a #GskLayer.
 *
 * This function is a convenience for setting the #GskLayer:position
 * and #GskLayer:bounds of a layer.
 *
 * Since: 3.18
 */
void
gsk_layer_set_frame (GskLayer              *self,
                     const graphene_rect_t *frame)
{
  GeometryInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (frame != NULL);

  info = gsk_layer_state_get_geometry_info (gsk_layer_get_state (self));

  info->bounds.origin.x = 0.f;
  info->bounds.origin.y = 0.f;
  info->bounds.size.width = frame->size.width;
  info->bounds.size.height = frame->size.height;

  info->position.x = frame->origin.x
                   + (info->bounds.origin.x + info->bounds.size.width)
                   * info->pivot_point.x;
  info->position.y = frame->origin.y
                   + (info->bounds.origin.y + info->bounds.size.height)
                   * info->pivot_point.y;

  gsk_layer_maybe_queue_relayout (self);
}

/**
 * gsk_layer_get_frame:
 * @self: a #GskLayer
 * @frame: (out caller-allocates): the frame rectangle
 *
 * Retrieves the frame rectangle of a #GskLayer.
 *
 * The rectangle is determined by the #GskLayer:bounds of the layer,
 * as well as its #GskLayer:position and #GskLayer:pivot-point.
 *
 * The frame is unaffected by transformations.
 *
 * Since: 3.18
 */
void
gsk_layer_get_frame (GskLayer        *self,
                     graphene_rect_t *frame)
{
  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (frame != NULL);

  gsk_layer_state_get_frame (gsk_layer_get_state (self), frame);

  /* If the layer has no parent, the origin is set to (0, 0) */
  if (GSK_LAYER_PRIV (self)->parent == NULL)
    graphene_point_init (&frame->origin, 0, 0);

  graphene_rect_round_to_pixel (frame);
}

/**
 * gsk_layer_get_preferred_size:
 * @self: a #GskLayer
 * @size: (out caller-allocates): the preferred size of the layer
 *
 * Retrieves the preferred size of a #GskLayer.
 *
 * If the layer does not override the #GskLayerClass.get_preferred_size
 * virtual function, this function returns the size of the #GskLayer:bounds
 * property.
 *
 * Since: 3.18
 */
void
gsk_layer_get_preferred_size (GskLayer        *self,
                              graphene_size_t *size)
{
  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (size != NULL);

  GSK_LAYER_GET_CLASS (self)->get_preferred_size (self, size);
}

void
gsk_layer_set_rotation (GskLayer *self,
                        const graphene_euler_t *rotation)
{
  TransformInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (rotation != NULL);

  info = gsk_layer_state_get_transform_info (gsk_layer_get_state (self));
  graphene_euler_init_from_euler (&info->rotation, rotation);
  info->needs_modelview_update = TRUE;
  info->needs_inverse_update = TRUE;

  gsk_layer_queue_redraw (self);
}

void
gsk_layer_get_rotation (GskLayer *self,
                        graphene_euler_t *rotation)
{
  const TransformInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (rotation != NULL);

  info = gsk_layer_state_peek_transform_info (gsk_layer_get_state (self));
  graphene_euler_init_from_euler (rotation, &info->rotation);
}

void
gsk_layer_set_scale (GskLayer *self,
                     float scale_x,
                     float scale_y)
{
  TransformInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));

  info = gsk_layer_state_get_transform_info (gsk_layer_get_state (self));
  graphene_vec3_init (&info->scale, scale_x, scale_y, graphene_vec3_get_z (&info->scale));

  info->needs_modelview_update = TRUE;
  info->needs_inverse_update = TRUE;

  gsk_layer_queue_redraw (self);
}

void
gsk_layer_get_scale (GskLayer *self,
                     float *scale_x,
                     float *scale_y)
{
  const TransformInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));

  info = gsk_layer_state_peek_transform_info (gsk_layer_get_state (self));
  if (scale_x != NULL)
    *scale_x = graphene_vec3_get_x (&info->scale);
  if (scale_y != NULL)
    *scale_y = graphene_vec3_get_y (&info->scale);
}

static inline void
gsk_layer_maybe_update_modelview (GskLayer *self)
{
  TransformInfo *tinfo;
#if 0
  const GeometryInfo *ginfo;
  graphene_point3d_t pivot, unpivot;

  ginfo = gsk_layer_state_peek_geometry_info (gsk_layer_get_state (self));
#endif

  tinfo = gsk_layer_state_get_transform_info (gsk_layer_get_state (self));

  if (!tinfo->needs_modelview_update)
    return;

  g_print (G_STRLOC ": Updating modelview...\n");

  /* Set the initial state of the modelview */
  if (GSK_LAYER_PRIV (self)->parent != NULL)
    {
      GskLayerState *parent_state;
      const TransformInfo *parent_info;

      parent_state = gsk_layer_get_state (GSK_LAYER_PRIV (self)->parent);
      parent_info = gsk_layer_state_get_transform_info (parent_state);

      graphene_matrix_init_from_matrix (&tinfo->modelview, &parent_info->child_transform);
    }
  else
    graphene_matrix_init_identity (&tinfo->modelview);

#if 0
  pivot.x = ginfo->pivot_point.x * (ginfo->bounds.origin.x + ginfo->bounds.size.width);
  pivot.y = ginfo->pivot_point.y * (ginfo->bounds.origin.y + ginfo->bounds.size.height);
  pivot.z = tinfo->pivot_z;

  unpivot.x = -pivot.x;
  unpivot.y = -pivot.y;
  unpivot.z = -pivot.z;

  graphene_matrix_translate (&tinfo->modelview, &pivot);
#endif

  /* If the transform matrix is unset, we apply the decomposed
   * transformations, otherwise we only apply the transform
   * matrix
   */
  if (graphene_matrix_is_identity (&tinfo->transform))
    {
      graphene_point3d_t translation =
        GRAPHENE_POINT3D_INIT (tinfo->translate.x,
                               tinfo->translate.y,
                               tinfo->translate.z);

      /* Apply the pivot translation (and the addition translation) */
      graphene_matrix_translate (&tinfo->modelview, &translation);

      /* Then the scale */
      graphene_matrix_scale (&tinfo->modelview,
                             graphene_vec3_get_x (&tinfo->scale),
                             graphene_vec3_get_y (&tinfo->scale),
                             graphene_vec3_get_z (&tinfo->scale));

      /* Then the rotation */
      {
        graphene_quaternion_t q;

        graphene_quaternion_init_from_euler (&q, &tinfo->rotation);
        graphene_matrix_rotate_quaternion (&tinfo->modelview, &q);
      }

      GSK_NOTE (GEOMETRY, g_print ("Applying decomposed translations to %s\n",
                                   gsk_layer_get_debug_name (self)));
    }
  else
    {
      /* Apply the transform matrix as is */
      GSK_NOTE (GEOMETRY, g_print ("Applying  transform to %s\n",
                                   gsk_layer_get_debug_name (self)));
      graphene_matrix_multiply (&tinfo->modelview,
                                &tinfo->transform,
                                &tinfo->modelview);
    }

#if 0
  /* Undo the pivot translation */
  graphene_matrix_translate (&tinfo->modelview, &unpivot);
#endif

  tinfo->needs_modelview_update = FALSE;
  tinfo->needs_inverse_update = TRUE;
}

const graphene_matrix_t *
gsk_layer_get_modelview (GskLayer *self)
{
  TransformInfo *info;

  gsk_layer_maybe_update_modelview (self);
  info = gsk_layer_state_get_transform_info (gsk_layer_get_state (self));
  g_assert (!info->needs_modelview_update);

  return &info->modelview;
}

const graphene_matrix_t *
gsk_layer_get_inverse_modelview (GskLayer *self)
{
  TransformInfo *info;

  gsk_layer_maybe_update_modelview (self);
  info = gsk_layer_state_get_transform_info (gsk_layer_get_state (self));
  if (!info->needs_inverse_update)
    return &info->inverse;

  graphene_matrix_inverse (&info->modelview, &info->inverse);
  info->needs_inverse_update = FALSE;

  return &info->inverse;
}

void
gsk_layer_set_background_color (GskLayer *self,
                                const GdkRGBA *background_color)
{
  RenderInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (background_color != NULL);

  info = gsk_layer_state_get_render_info (gsk_layer_get_state (self));
  info->background_color = *background_color;
  gsk_layer_queue_redraw (self);
}

void
gsk_layer_set_opacity (GskLayer *self,
                       double    opacity)
{
  RenderInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));

  info = gsk_layer_state_get_render_info (gsk_layer_get_state (self));
  info->opacity = CLAMP (opacity, 0, 1);
  gsk_layer_queue_redraw (self);
}

void
gsk_layer_set_clip (GskLayer              *self,
                    const graphene_rect_t *clip)
{
  RenderInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));

  info = gsk_layer_state_get_render_info (gsk_layer_get_state (self));
  if (clip == NULL)
    {
      info->use_clip = FALSE;
      return;
    }

  graphene_rect_init_from_rect (&info->clip, clip);
  info->use_clip = TRUE;

  gsk_layer_queue_redraw (self);
}

void
gsk_layer_get_clip (GskLayer        *self,
                    graphene_rect_t *clip)
{
  const RenderInfo *info;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (clip != NULL);

  info = gsk_layer_state_peek_render_info (gsk_layer_get_state (self));
  if (info->use_clip)
    {
      graphene_rect_init_from_rect (clip, &info->clip);
      return;
    }

  gsk_layer_get_bounds (self, clip);
}

static void
gsk_layer_revalidate_layout (GskLayer *root)
{
  if (!GSK_LAYER_PRIV (root)->needs_relayout)
    return;

  if (GSK_LAYER_PRIV (root)->in_layout)
    return;

  GSK_LAYER_PRIV (root)->in_layout = TRUE;

  GSK_LAYER_PRIV (root)->needs_relayout = FALSE;

  if (GSK_LAYER_PRIV (root)->n_children != 0)
    g_signal_emit (root, layer_signals[LAYOUT_CHILDREN], 0);

  GSK_LAYER_PRIV (root)->in_layout = FALSE;
}

void
gsk_layer_set_frame_clock (GskLayer *self,
                           GdkFrameClock *frame_clock)
{
  GskLayerPrivate *priv = gsk_layer_get_instance_private (self);
  GdkFrameClock *old_frame_clock;

  g_return_if_fail (GSK_IS_LAYER (self));
  g_return_if_fail (priv->parent == NULL);
  g_return_if_fail (frame_clock == NULL || GDK_IS_FRAME_CLOCK (frame_clock));

  old_frame_clock = priv->frame_clock;

  if (g_set_object (&priv->frame_clock, frame_clock))
    {
      if (old_frame_clock != NULL)
        g_signal_handlers_disconnect_by_func (old_frame_clock,
                                              gsk_layer_revalidate_layout,
                                              self);

      if (priv->frame_clock != NULL)
        {
          g_signal_connect_object (priv->frame_clock, "layout",
                                   G_CALLBACK (gsk_layer_revalidate_layout),
                                   self,
                                   G_CONNECT_SWAPPED);
          gsk_layer_queue_relayout (self);
        }
    }
}
