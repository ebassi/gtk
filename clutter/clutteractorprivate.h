#ifndef __CLUTTER_ACTOR_PRIVATE_H__
#define __CLUTTER_ACTOR_PRIVATE_H__

#include <clutter/clutteractor.h>

G_BEGIN_DECLS

/* convenience macro for generating direct getters */
#define CLUTTER_ACTOR_DEFINE_GET(TypeName, type_name, FieldType, field_name) \
FieldType \
type_name##_get_##field_name (TypeName *actor) \
{ \
  TypeName##Private *priv = type_name##_get_instance_private (actor); \
\
  g_return_val_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (actor, type_name##_get_type ()), 0); \
\
  return priv->field_name; \
}

G_END_DECLS

#endif /* __CLUTTER_ACTOR_PRIVATE_H__ */
