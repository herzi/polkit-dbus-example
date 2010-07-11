#include <stdio.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <polkit/polkit.h>

#include "mirror.h"
#include "mirror-server-glue.h"


/* Boilerplate for Mirror object */
G_DEFINE_TYPE(Mirror, mirror, G_TYPE_OBJECT)
Mirror *mirror_new() { return g_object_new(TYPE_MIRROR, NULL); }
static void mirror_init(Mirror *mirror) {}
static void
mirror_class_init(MirrorClass *klass)
{
    /* Add D-Bus metadata */
    dbus_g_object_type_install_info(TYPE_MIRROR, &dbus_glib_mirror_object_info);
}


/* Method expored over D-Bus and uses PolKit for authorization */
gboolean
mirror_reflect(Mirror *mirror, const gchar *name, DBusGMethodInvocation *context)
{
    gchar *value;
    GError *error = NULL;
    PolkitAuthorizationResult *result;
    
    printf("reflect '%s' by %s\n", name, dbus_g_method_get_sender(context));

    result = polkit_authority_check_authorization_sync(polkit_authority_get(),
                                                       polkit_system_bus_name_new(dbus_g_method_get_sender(context)),
                                                       "com.example.mirror.use",
                                                       NULL,
                                                       POLKIT_CHECK_AUTHORIZATION_FLAGS_ALLOW_USER_INTERACTION,
                                                       NULL,
                                                       &error);
    if (error) {
        printf("Failed to check authorization: %s\n", error->message);
        dbus_g_method_return_error(context, error);
        return FALSE;
    }
    if (!polkit_authorization_result_get_is_authorized(result)) {
        printf("Not authorized\n");
        error = g_error_new(DBUS_GERROR_REMOTE_EXCEPTION, 0, "Not authorized");
        dbus_g_method_return_error(context, error);
        return FALSE;
    }
    
    value = g_strdup(name);
    g_strreverse(value);

    dbus_g_method_return(context, value);
    
    return TRUE;
}


int main(int argc, char **argv)
{
    GError *error = NULL;
    DBusGConnection *connection;
    DBusError dbus_error;
    Mirror *mirror;
    GMainLoop *loop;
    int result;

    /* Initialise GLib */
    g_type_init();
    
    /* Connect to the system D-Bus */
    connection = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
    if (error) {
        printf("Failed to get bus: %s\n", error->message);
        return 1;
    }
    
    /* Register this services name on the bus */
    dbus_error_init(&dbus_error);
    result = dbus_bus_request_name(dbus_g_connection_get_connection(connection),
                                    "com.example.Mirror", DBUS_NAME_FLAG_DO_NOT_QUEUE, &dbus_error);
    if (dbus_error_is_set(&dbus_error)) {
        printf("Failed to register name: %s\n", dbus_error.message);
        return 1;
    }
    if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        printf("Unable to get bus name\n");
        return 1;
    }
    
    /* Make a new object and connect it to the bus */
    mirror = mirror_new();
    dbus_g_connection_register_g_object(connection, "/com/example/Mirror", G_OBJECT(mirror));

    /* Loop, processing requests */
    loop = g_main_loop_new(g_main_context_default(), TRUE);
    g_main_loop_run(loop);

    return 0;
}

