#ifndef __CLUTTER_MACROS_H__
#define __CLUTTER_MACROS_H__

#if !defined (__CLUTTER_H_INSIDE__) && !defined (CLUTTER_COMPILATION)
#error "Only <clutter/clutter.h> can be included directly."
#endif

#ifdef CLUTTER_COMPILATION
#define CLUTTER_PRIVATE_FIELD(TypeName,field_name) \
  TypeName field_name
#else
#define CLUTTER_PRIVATE_FIELD(TypeName,field_name) \
  TypeName clutter_private__##field_name
#endif

#define _CLUTTER_EXTERN         _GDK_EXTERN

#ifdef GDK_DISABLE_DEPRECATION_WARNINGS
#define CLUTTER_DEPRECATED _CLUTTER_EXTERN
#define CLUTTER_DEPRECATED_FOR(f) _CLUTTER_EXTERN
#define CLUTTER_UNAVAILABLE(maj,min) _CLUTTER_EXTERN
#else
#define CLUTTER_DEPRECATED G_DEPRECATED _CLUTTER_EXTERN
#define CLUTTER_DEPRECATED_FOR(f) G_DEPRECATED_FOR(f) _CLUTTER_EXTERN
#define CLUTTER_UNAVAILABLE(maj,min) G_UNAVAILABLE(maj,min) _CLUTTER_EXTERN
#endif

/* Used for enumeration types */
#define CLUTTER_AVAILABLE_IN_ALL                _CLUTTER_EXTERN

/* XXX: Every new stable minor release bump should add a macro here */

#if GDK_VERSION_MIN_REQUIRED >= GDK_VERSION_3_12
# define CLUTTER_DEPRECATED_IN_3_12             CLUTTER_DEPRECATED
# define CLUTTER_DEPRECATED_IN_3_12_FOR(f)      CLUTTER_DEPRECATED_FOR(f)
#else
# define CLUTTER_DEPRECATED_IN_3_12             _CLUTTER_EXTERN
# define CLUTTER_DEPRECATED_IN_3_12_FOR(f)      _CLUTTER_EXTERN
#endif

#if GDK_VERSION_MAX_ALLOWED < GDK_VERSION_3_12
# define CLUTTER_AVAILABLE_IN_3_12              CLUTTER_UNAVAILABLE(3, 12)
#else
# define CLUTTER_AVAILABLE_IN_3_12              _CLUTTER_EXTERN
#endif

#endif /* __CLUTTER_MACROS_H__ */
