#include <sys/types.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <dbus/dbus.h>
#if 0
#include <polkit-dbus/polkit-dbus.h>
#endif

#include "mirror-client-glue.h"

GtkWidget *error_label;

DBusGProxy *proxy = NULL;

static void
mirror_cb(GtkWidget *entry)
{
    char *result;
    GError *error = NULL;
    
    com_example_Mirror_reflect(proxy,
                               gtk_entry_get_text(GTK_ENTRY(entry)),
                               &result,
                               &error);
    if (error != NULL) {
        gtk_label_set(GTK_LABEL(error_label), error->message);
        return;
    }

    gtk_label_set(GTK_LABEL(error_label), "");
    gtk_entry_set_text(GTK_ENTRY(entry), result);
}

#if 0
static void
authorize_cb(GtkWidget *button)
{   
    DBusConnection *bus;
    DBusError dbus_error;
    PolKitContext *context;
    PolKitAction *action;
    PolKitCaller *caller;
    PolKitResult result;
    polkit_bool_t authorized;
    PolKitError *polkit_error = NULL;
    
    //const gchar *action_name = "com.example.mirror.use";
    const gchar *action_name = "org.gnome.clockapplet.mechanism.settimezone";
    
    dbus_error_init(&dbus_error);
    bus = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
    caller = polkit_caller_new_from_pid(bus, getpid(), &dbus_error);
    
    context = polkit_context_new();
    polkit_context_init(context, &polkit_error);
    action = polkit_action_new();
    polkit_action_set_action_id(action, action_name);
    result = polkit_context_is_caller_authorized(context, action, caller, TRUE, &polkit_error);
    printf("%s\n", polkit_result_to_string_representation(result));

    authorized = polkit_auth_obtain(action_name, 0, getpid(), &dbus_error);
    if (dbus_error_is_set(&dbus_error)) {
        printf("Error authorizing: %s\n", dbus_error.message);
        return;
    }
    printf("authorized = %s\n", authorized ? "yes" : "no");

    /* Disable button once authorized */
    if(authorized)
        gtk_widget_set_sensitive(button, FALSE);
}
#endif

int main(int argc, char **argv)
{
    DBusGConnection *connection;    
    GtkWidget *window, *vbox, *entry, *label, *button;
    GError *error = NULL;
    
    gtk_init(&argc, &argv);
    
    connection = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
    if (error) {
        printf("Failed to get bus: %s\n", error->message);
        return 1;
    }
    proxy = dbus_g_proxy_new_for_name(connection,
                                      "com.example.Mirror",
                                      "/com/example/Mirror",
                                      "com.example.Mirror");

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    label = gtk_label_new("Text to send:");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 3);
    
    entry = gtk_entry_new();
    g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(mirror_cb), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, TRUE, 3);
    
    error_label = gtk_label_new("");
    gtk_label_set_line_wrap(GTK_LABEL(error_label), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), error_label, FALSE, TRUE, 3);

#if 0
    button = gtk_button_new_with_label("Unlock");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(authorize_cb), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, TRUE, 3);
#endif

    gtk_widget_show_all(window);
    
    gtk_main();
    
    return 0;
}

