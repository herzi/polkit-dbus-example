#include <sys/types.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <dbus/dbus.h>
#include <polkitgtk/polkitgtk.h>

#include "mirror-client-glue.h"

GtkWidget *error_label;

DBusGProxy *proxy = NULL;

static void
handle_reply (DBusGProxy * proxy,
              char       * OUT_out,
              GError     * error,
              gpointer     entry)
{
    if (error) {
        gtk_label_set_text (GTK_LABEL (error_label),
                            error->message);
        /* FIXME: free the error? */
    } else {
        gtk_label_set(GTK_LABEL(error_label), "");

        gtk_entry_set_text(GTK_ENTRY(entry), OUT_out);
    }

    gtk_widget_set_sensitive (entry, TRUE);
}

static void
mirror_cb(GtkWidget *entry)
{
    gtk_widget_set_sensitive (entry, FALSE);
    com_example_Mirror_reflect_async (proxy, gtk_entry_get_text (GTK_ENTRY (entry)), handle_reply, entry);
}


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

    button = polkit_lock_button_new ("com.example.mirror.use");
    polkit_lock_button_set_unlock_text (POLKIT_LOCK_BUTTON (button), "sliff");
    polkit_lock_button_set_lock_text (POLKIT_LOCK_BUTTON (button), "sloff");
    polkit_lock_button_set_lock_down_text (POLKIT_LOCK_BUTTON (button), "foo");
    polkit_lock_button_set_not_authorized_text (POLKIT_LOCK_BUTTON (button), "bar");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, TRUE, 3);

    g_print ("%s\n", polkit_lock_button_get_is_visible (POLKIT_LOCK_BUTTON (button)) ? "visible" : "invisible");

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

