#ifndef __GSK_LAYER_CONTENT_PRIVATE_H__
#define __GSK_LAYER_CONTENT_PRIVATE_H__

#include "gsklayercontent.h"

G_BEGIN_DECLS

void            gsk_layer_content_draw                  (GskLayerContent *self,
                                                         GskLayer        *layer,
                                                         cairo_t         *cr);
gboolean        gsk_layer_content_get_preferred_size    (GskLayerContent *self,
                                                         graphene_size_t *size);
gboolean        gsk_layer_content_needs_redraw          (GskLayerContent *self);

G_END_DECLS

#endif /* __GSK_LAYER_CONTENT_PRIVATE_H__ */
