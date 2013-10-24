#ifndef __CLUTTER_TYPES_H__
#define __CLUTTER_TYPES_H__

#if !defined (__CLUTTER_H_INSIDE__) && !defined (CLUTTER_COMPILATION)
#error "Only <clutter/clutter.h> can be included directly."
#endif

#include <glib-object.h>
#include <gdk/gdk.h>
#include <clutter/cluttermacros.h>

G_BEGIN_DECLS

typedef struct _ClutterActor            ClutterActor;

typedef struct _ClutterPoint            ClutterPoint;
typedef struct _ClutterSize             ClutterSize;
typedef struct _ClutterRect             ClutterRect;

/*
 * ClutterPoint
 */

#define CLUTTER_TYPE_POINT              (clutter_point_get_type ())

/**
 * CLUTTER_POINT_INIT:
 * @x: X coordinate
 * @y: Y coordinate
 *
 * A simple macro for initializing a #ClutterPoint when declaring it, e.g.:
 *
 * |[
 *   ClutterPoint p = CLUTTER_POINT_INIT (100, 100);
 * ]|
 *
 * Since: 3.12
 */
#define CLUTTER_POINT_INIT(x,y)         { (x), (y) }

/**
 * CLUTTER_POINT_INIT_ZERO:
 *
 * A simple macro for initializing a #ClutterPoint to (0, 0) when
 * declaring it.
 *
 * Since: 3.12
 */
#define CLUTTER_POINT_INIT_ZERO         CLUTTER_POINT_INIT (0.f, 0.f)

/**
 * ClutterPoint:
 * @x: X coordinate, in pixels
 * @y: Y coordinate, in pixels
 *
 * A point in 2D space.
 *
 * Since: 3.12
 */
struct _ClutterPoint
{
  float x;
  float y;
};

CLUTTER_AVAILABLE_IN_3_12
GType clutter_point_get_type (void) G_GNUC_CONST;

CLUTTER_AVAILABLE_IN_3_12
const ClutterPoint *    clutter_point_zero              (void);

CLUTTER_AVAILABLE_IN_3_12
ClutterPoint *          clutter_point_alloc             (void);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_point_free              (ClutterPoint       *point);
CLUTTER_AVAILABLE_IN_3_12
ClutterPoint *          clutter_point_init              (ClutterPoint       *point,
                                                         float               x,
                                                         float               y);
CLUTTER_AVAILABLE_IN_3_12
ClutterPoint *          clutter_point_init_with_point   (ClutterPoint       *point,
                                                         const ClutterPoint *src);
CLUTTER_AVAILABLE_IN_3_12
gboolean                clutter_point_equal             (gconstpointer       v1,
                                                         gconstpointer       v2);
CLUTTER_AVAILABLE_IN_3_12
float                   clutter_point_distance          (const ClutterPoint *p1,
                                                         const ClutterPoint *p2,
                                                         float              *delta_x,
                                                         float              *delta_y);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_point_interpolate       (const ClutterPoint *p1,
                                                         const ClutterPoint *p2,
                                                         double              factor,
                                                         ClutterPoint       *res);

/*
 * ClutterSize
 */

#define CLUTTER_TYPE_SIZE               (clutter_size_get_type ())

/**
 * CLUTTER_SIZE_INIT:
 * @width: the width
 * @height: the height
 *
 * A simple macro for initializing a #ClutterSize when declaring it, e.g.:
 *
 * |[
 *   ClutterSize s = CLUTTER_SIZE_INIT (200, 200);
 * ]|
 *
 * Since: 3.12
 */

#define CLUTTER_SIZE_INIT(w,h)          { (w), (h) }

/**
 * CLUTTER_SIZE_INIT_ZERO:
 *
 * A simple macro for initializing a #ClutterSize to (0, 0) when
 * declaring it.
 *
 * Since: 3.12
 */
#define CLUTTER_SIZE_INIT_ZERO          CLUTTER_SIZE_INIT (0.f, 0.f)

/**
 * ClutterSize:
 * @width: the width, in pixels
 * @height: the height, in pixels
 *
 * A size, in 2D space.
 *
 * Since: 3.12
 */
struct _ClutterSize
{
  float width;
  float height;
};

CLUTTER_AVAILABLE_IN_3_12
GType clutter_size_get_type (void) G_GNUC_CONST;

CLUTTER_AVAILABLE_IN_3_12
ClutterSize *           clutter_size_alloc              (void);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_size_free               (ClutterSize       *size);
CLUTTER_AVAILABLE_IN_3_12
ClutterSize *           clutter_size_init               (ClutterSize       *size,
                                                         float              width,
                                                         float              height);
CLUTTER_AVAILABLE_IN_3_12
ClutterSize *           clutter_size_init_with_size     (ClutterSize       *size,
                                                         const ClutterSize *src);
CLUTTER_AVAILABLE_IN_3_12
gboolean                clutter_size_equal              (gconstpointer      v1,
                                                         gconstpointer      v2);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_size_interpolate        (const ClutterSize *s1,
                                                         const ClutterSize *s2,
                                                         double             factor,
                                                         ClutterSize       *res);

/*
 * ClutterRect
 */

#define CLUTTER_TYPE_RECT               (clutter_rect_get_type ())

/**
 * CLUTTER_RECT_INIT:
 * @x: the X coordinate
 * @y: the Y coordinate
 * @w: the width
 * @h: the height
 *
 * A simple macro for initializing a #ClutterRect when declaring it, e.g.:
 *
 * |[
 *   ClutterRect r = CLUTTER_RECT_INIT (100, 100, 200, 200);
 * ]|
 *
 * Since: 3.12
 */
#define CLUTTER_RECT_INIT(x,y,w,h)      { { (x), (y) }, { (w), (h) } }

/**
 * CLUTTER_RECT_INIT_ZERO:
 *
 * A simple macro for initializing a #ClutterRect to (0, 0, 0, 0) when
 * declaring it.
 *
 * Since: 3.12
 */
#define CLUTTER_RECT_INIT_ZERO          CLUTTER_RECT_INIT (0.f, 0.f, 0.f, 0.f)

/**
 * ClutterRect:
 * @origin: the origin of the rectangle
 * @size: the size of the rectangle
 *
 * The location and size of a rectangle.
 *
 * The width and height of a #ClutterRect can be negative; Clutter considers
 * a rectangle with an origin of [ 0.0, 0.0 ] and a size of [ 10.0, 10.0 ] to
 * be equivalent to a rectangle with origin of [ 10.0, 10.0 ] and size of
 * [ -10.0, -10.0 ].
 *
 * Application code can normalize rectangles using clutter_rect_normalize():
 * this function will ensure that the width and height of a #ClutterRect are
 * positive values. All functions taking a #ClutterRect as an argument will
 * implicitly normalize it before computing eventual results. For this reason
 * it is safer to access the contents of a #ClutterRect by using the provided
 * API at all times, instead of directly accessing the structure members.
 *
 * Since: 3.12
 */
struct _ClutterRect
{
  ClutterPoint origin;

  ClutterSize size;
};

CLUTTER_AVAILABLE_IN_3_12
GType clutter_rect_get_type (void) G_GNUC_CONST;

CLUTTER_AVAILABLE_IN_3_12
const ClutterRect *     clutter_rect_zero               (void);

CLUTTER_AVAILABLE_IN_3_12
ClutterRect *           clutter_rect_alloc              (void);
CLUTTER_AVAILABLE_IN_3_12
ClutterRect *           clutter_rect_init               (ClutterRect       *rect,
                                                         float              x,
                                                         float              y,
                                                         float              width,
                                                         float              height);
CLUTTER_AVAILABLE_IN_3_12
ClutterRect *           clutter_rect_init_with_rect     (ClutterRect       *rect,
                                                         const ClutterRect *src);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_rect_free               (ClutterRect       *rect);
CLUTTER_AVAILABLE_IN_3_12
gboolean                clutter_rect_equal              (ClutterRect       *a,
                                                         ClutterRect       *b);

CLUTTER_AVAILABLE_IN_3_12
ClutterRect *           clutter_rect_normalize          (ClutterRect       *rect);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_rect_get_center         (ClutterRect       *rect,
                                                         ClutterPoint      *center);
CLUTTER_AVAILABLE_IN_3_12
gboolean                clutter_rect_contains_point     (ClutterRect       *rect,
                                                         ClutterPoint      *point);
CLUTTER_AVAILABLE_IN_3_12
gboolean                clutter_rect_contains_rect      (ClutterRect       *a,
                                                         ClutterRect       *b);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_rect_union              (ClutterRect       *a,
                                                         ClutterRect       *b,
                                                         ClutterRect       *res);
CLUTTER_AVAILABLE_IN_3_12
gboolean                clutter_rect_intersection       (ClutterRect       *a,
                                                         ClutterRect       *b,
                                                         ClutterRect       *res);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_rect_offset             (ClutterRect       *rect,
                                                         float              d_x,
                                                         float              d_y);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_rect_inset              (ClutterRect       *rect,
                                                         float              d_x,
                                                         float              d_y);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_rect_clamp_to_pixel     (ClutterRect       *rect);
CLUTTER_AVAILABLE_IN_3_12
float                   clutter_rect_get_x              (ClutterRect       *rect);
CLUTTER_AVAILABLE_IN_3_12
float                   clutter_rect_get_y              (ClutterRect       *rect);
CLUTTER_AVAILABLE_IN_3_12
float                   clutter_rect_get_width          (ClutterRect       *rect);
CLUTTER_AVAILABLE_IN_3_12
float                   clutter_rect_get_height         (ClutterRect       *rect);
CLUTTER_AVAILABLE_IN_3_12
void                    clutter_rect_interpolate        (const ClutterRect *r1,
                                                         const ClutterRect *r2,
                                                         double             factor,
                                                         ClutterRect       *res);

G_END_DECLS

#endif /* __CLUTTER_TYPES_H__ */
