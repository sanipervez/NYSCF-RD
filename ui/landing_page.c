#include <gtk/gtk.h>
#include "landing_page.h"

#include <gtk/gtk.h>
#include "landing_page.h"

GtkWidget* create_landing_page() {
    // Main container box (vertical)
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Create label
    GtkWidget *welcome = gtk_label_new("Tri-Gas Controller");
    gtk_widget_set_name(welcome, "landing-title");

    // Create buttons container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *oxygen_button = gtk_button_new_with_label("Oxygen");
    gtk_widget_set_size_request(oxygen_button, 200, 100);  // control size

    GtkWidget *co2_button = gtk_button_new_with_label("CO₂");
    gtk_widget_set_size_request(co2_button, 200, 100);     // control size

    // Align Oxygen and CO2 buttons to start (left)
    gtk_widget_set_halign(oxygen_button, GTK_ALIGN_START);
    gtk_widget_set_halign(co2_button, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(vbox), oxygen_button, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), co2_button, FALSE, FALSE, 5);

    // Create the up arrow button
    GtkWidget *up_button = gtk_button_new();
    GtkWidget *up_image = gtk_image_new_from_icon_name("go-up", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(up_button), up_image);
    gtk_widget_set_size_request(up_button, 100, 80);  // smaller size

    // Create the down arrow button
    GtkWidget *down_button = gtk_button_new();
    GtkWidget *down_image = gtk_image_new_from_icon_name("go-down", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(down_button), down_image);
    gtk_widget_set_size_request(down_button, 100, 80);

    // Align arrow buttons to end (right)
    gtk_widget_set_halign(up_button, GTK_ALIGN_END);
    gtk_widget_set_halign(down_button, GTK_ALIGN_END);

    // Pack the arrow buttons below the others in the same vbox
    gtk_box_pack_start(GTK_BOX(vbox), up_button, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), down_button, FALSE, FALSE, 5);

    // Pack label and buttons container into main box
    gtk_box_pack_start(GTK_BOX(box), welcome, TRUE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(box), vbox, TRUE, TRUE, 10);

    return box;
}


