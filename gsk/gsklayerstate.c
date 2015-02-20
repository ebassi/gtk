#include "config.h"

#include "gsklayerprivate.h"

GskLayerState *
gsk_layer_state_copy (GskLayerState *src)
{
  GskLayerState *dest;

  if (src == NULL)
    return NULL;

  dest = g_slice_new (GskLayerState);
  dest->geometry_info = g_slice_dup (GeometryInfo, src->geometry_info);
  dest->transform_info = g_slice_dup (TransformInfo, src->transform_info);
  dest->render_info = g_slice_dup (RenderInfo, src->render_info);

  return dest;
}

void
gsk_layer_state_clear (GskLayerState *state)
{
  g_slice_free (GeometryInfo, state->geometry_info);
  g_slice_free (TransformInfo, state->transform_info);
  g_slice_free (RenderInfo, state->render_info);
}

void
gsk_layer_state_free (GskLayerState *state)
{
  if (state != NULL)
    {
      gsk_layer_state_clear (state);

      g_slice_free (GskLayerState, state);
    }
}

static const GeometryInfo default_geometry_info = {
  /* .bounds = */
  GRAPHENE_RECT_INIT (0, 0, 0, 0),

  /* .pivot_point = */
  GRAPHENE_POINT_INIT (0.5, 0.5),

  /* .position = */
  GRAPHENE_POINT_INIT (0, 0),
};

static const RenderInfo default_render_info = {
  /* .background_color = */
  { 0., 0., 0., 1. },

  /* .opacity = */
  1.,

  /* .clip = */
  GRAPHENE_RECT_INIT (0, 0, 0, 0),

  /* .use_clip = */
  FALSE,
};

/*< private >
 * gsk_layer_state_peek_geometry_info:
 * @state: a #GskLayerState
 *
 * Retrieves a read-only pointer to the layer's geometry info.
 *
 * Use this function when implementing getter functions.
 *
 * Returns: (transfer none): the geometry info of a particular layer state
 */
const GeometryInfo *
gsk_layer_state_peek_geometry_info (GskLayerState *state)
{
  if (state->geometry_info == NULL)
    return &default_geometry_info;

  return state->geometry_info;
}

/*< private >
 * gsk_layer_state_get_geometry_info:
 * @state: a #GskLayerState
 *
 * Retrieves a pointer to the layer's geometry info.
 *
 * Use this function when implementing setter functions.
 *
 * Returns: (transfer none): the geometry info of a particular layer state
 */
GeometryInfo *
gsk_layer_state_get_geometry_info (GskLayerState *state)
{
  if (state->geometry_info == NULL)
    state->geometry_info = g_slice_dup (GeometryInfo, &default_geometry_info);

  return state->geometry_info;
}

static void
transform_info_init (TransformInfo *info)
{
  graphene_euler_init (&info->rotation, 0.f, 0.f, 0.f);
  graphene_vec3_init (&info->scale, 1.f, 1.f, 1.f);
  graphene_point3d_init (&info->translate, 0.f, 0.f, 0.f);

  info->pivot_z = 0.f;

  graphene_matrix_init_identity (&info->transform);
  graphene_matrix_init_identity (&info->child_transform);

  graphene_matrix_init_identity (&info->modelview);
  graphene_matrix_init_identity (&info->inverse);

  info->needs_modelview_update = FALSE;
  info->needs_inverse_update = FALSE;
}

/*< private >
 * gsk_layer_state_peek_transform_info:
 * @state: a #GskLayerState
 *
 * Retrieves a read-only pointer to the layer's transformation info.
 *
 * Use this function when implementing getters.
 *
 * Returns: (transfer none): the transformation info of a particular layer state
 */
const TransformInfo *
gsk_layer_state_peek_transform_info (GskLayerState *state)
{
  static TransformInfo default_transform_info;
  static gboolean default_transform_info_set;

  if (G_UNLIKELY (!default_transform_info_set))
    transform_info_init (&default_transform_info);

  if (state->transform_info == NULL)
    return &default_transform_info;

  return state->transform_info;
}

/*< private >
 * gsk_layer_state_get_transform_info:
 * @state: a #GskLayerState
 *
 * Retrieves a pointer to the layer's transformation info.
 *
 * Returns: (transfer none): the transformation info of a particular layer state
 */
TransformInfo *
gsk_layer_state_get_transform_info (GskLayerState *state)
{
  if (state->transform_info == NULL)
    {
      state->transform_info = g_slice_new (TransformInfo);
      transform_info_init (state->transform_info);
    }

  return state->transform_info;
}

/*< private >
 * gsk_layer_state_peek_render_info:
 * @state: a #GskLayerState
 *
 * Retrieves a read-only pointer to the layer's rendering info.
 *
 * Use this function when implementing getters.
 *
 * Returns: (transfer none): the rendering info of a particular layer state
 */
const RenderInfo *
gsk_layer_state_peek_render_info (GskLayerState *state)
{
  if (state->render_info == NULL)
    return &default_render_info;

  return state->render_info;
}

/*< private >
 * gsk_layer_state_get_render_info:
 * @state: a #GskLayerState
 *
 * Retrieves a pointer to the layer's rendering info.
 *
 * Use this function when implementing setters.
 *
 * Returns: (transfer none): the rendering info of a particular layer state
 */
RenderInfo *
gsk_layer_state_get_render_info (GskLayerState *state)
{
  if (state->render_info == NULL)
    state->render_info = g_slice_dup (RenderInfo, &default_render_info);

  return state->render_info;
}

void
gsk_layer_state_get_frame (GskLayerState   *state,
                           graphene_rect_t *frame)
{
  const GeometryInfo *info = gsk_layer_state_peek_geometry_info (state);

  frame->origin.x = info->position.x
                  - (info->bounds.origin.x + info->bounds.size.width)
                  * info->pivot_point.x;
  frame->origin.y = info->position.y
                  - (info->bounds.origin.y + info->bounds.size.height)
                  * info->pivot_point.y;

  frame->size.width = info->bounds.size.width;
  frame->size.height = info->bounds.size.height;
}
