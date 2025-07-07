#include <gtk/gtk.h>
#include "landing_page.h"

GtkWidget* create_landing_page() {
    // Create main vertical box
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Title label
    GtkWidget *welcome = gtk_label_new("Setpoint");
    gtk_widget_set_name(welcome, "landing-title");

    // Create grid for side-by-side buttons
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);

    // Create CO2 button
    GtkWidget *co2_button = gtk_button_new_with_label("% Carbon Dioxide");
    gtk_widget_set_name(co2_button, "co2_btn");
    gtk_widget_set_hexpand(co2_button, TRUE);
    gtk_widget_set_vexpand(co2_button, TRUE);

    // Create Oxygen button
    GtkWidget *oxygen_button = gtk_button_new_with_label("% Oxygen");
    gtk_widget_set_name(oxygen_button, "oxygen_btn");
    gtk_widget_set_hexpand(oxygen_button, TRUE);
    gtk_widget_set_vexpand(oxygen_button, TRUE);

    // Place buttons in grid
    gtk_grid_attach(GTK_GRID(grid), co2_button, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), oxygen_button, 1, 0, 1, 1);

    // Pack into main_vbox
    gtk_box_pack_start(GTK_BOX(main_vbox), welcome, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(main_vbox), grid, TRUE, TRUE, 0);

    return main_vbox;
}
