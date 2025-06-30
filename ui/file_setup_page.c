#include <gtk/gtk.h>
#include "file_setup_page.h"

GtkWidget* create_file_setup_page() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *welcome = gtk_label_new("File Setup Page (Coming Soon)");
    gtk_widget_set_name(welcome, "file-setup-title");

    gtk_box_pack_start(GTK_BOX(box), welcome, TRUE, TRUE, 10);

    return box;
}
