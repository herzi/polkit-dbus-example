#ifndef __MIRROR_H
#define __MIRROR_H

#include <glib-object.h>
#include <dbus/dbus-glib.h>
        
G_BEGIN_DECLS

#define TYPE_MIRROR         (mirror_get_type ())
#define MIRROR(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), TYPE_MIRROR, Mirror))
#define MIRROR_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), TYPE_MIRROR, MirrorClass))
#define IS_MIRROR(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), TYPE_MIRROR))
#define IS_MIRROR_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), TYPE_MIRROR))
#define MIRROR_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TYPE_MIRROR, MirrorClass))

typedef struct
{
    GObject parent;
} Mirror;

typedef struct
{
    GObjectClass parent_class;
} MirrorClass;

GType mirror_get_type();

Mirror *mirror_new();

gboolean mirror_reflect(Mirror *mirror, const gchar *name, DBusGMethodInvocation *context);

#endif /* __MIRROR_H */

