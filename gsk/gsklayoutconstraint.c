#include "config.h"

#include "gsklayoutconstraint.h"
#include "gskenumtypes.h"

struct _GskLayoutConstraint
{
  GObject parent_instance;

  GskLayer *source;
  GskLayer *target;

  GskLayoutAttribute source_attribute;
  GskLayoutAttribute target_attribute;

  float constant;
  float multiplier;
};

struct _GskLayoutConstraintClass
{
  GObjectClass parent_class;
};

G_DEFINE_TYPE (GskLayoutConstraint, gsk_layout_constraint, G_TYPE_OBJECT)

static void
gsk_layout_constraint_class_init (GskLayoutConstraintClass *klass)
{
}

static void
gsk_layout_constraint_init (GskLayoutConstraint *self)
{
}
