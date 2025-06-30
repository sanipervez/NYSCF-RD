#include <gtk/gtk.h>
#include "passcode_page.h"


GtkWidget* create_passcode_page() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Create the image
    GtkWidget *logo = gtk_image_new_from_file("/home/avathompson/Downloads/testIMG1");

    // Wrap the image in an event box to apply opacity
    GtkWidget *logo_container = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(logo_container), logo);
    gtk_widget_set_name(logo_container, "translucent-logo");

    // Create label
    GtkWidget *welcome = gtk_label_new("Passcode Page (Coming Soon)");
    gtk_widget_set_name(welcome, "passcode-title");

    // Pack the widgets into the box
    gtk_box_pack_start(GTK_BOX(box), logo_container, TRUE, TRUE, 20);
    gtk_box_pack_start(GTK_BOX(box), welcome, TRUE, TRUE, 10);

    return box;
}
