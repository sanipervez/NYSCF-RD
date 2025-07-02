#include <gtk/gtk.h>
#include "landing_page.h"

GtkWidget* create_landing_page() {
    // Main container
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Title
    GtkWidget *welcome = gtk_label_new("");
    gtk_widget_set_name(welcome, "landing-title");

    // Grid layout
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    // Gas buttons
    GtkWidget *oxygen_button = gtk_button_new_with_label("Oxygen");
    gtk_widget_set_size_request(oxygen_button, 200, 100);

    GtkWidget *co2_button = gtk_button_new_with_label("CO₂");
    gtk_widget_set_size_request(co2_button, 200, 100);

    // Reading displays
    GtkWidget *oxygen_reading = gtk_label_new("0.0%");
    gtk_widget_set_size_request(oxygen_reading, 100, 80);
    gtk_widget_set_name(oxygen_reading, "reading-label");

    GtkWidget *co2_reading = gtk_label_new("0.0%");
    gtk_widget_set_size_request(co2_reading, 100, 80);
    gtk_widget_set_name(co2_reading, "reading-label");

    // Arrow buttons
    GtkWidget *up_button = gtk_button_new();
    GtkWidget *up_image = gtk_image_new_from_icon_name("go-up", GTK_ICON_SIZE_BUTTON);
    gtk_widget_set_name(up_image, "up-img"); 
    gtk_button_set_image(GTK_BUTTON(up_button), up_image);
    gtk_widget_set_size_request(up_button, 100, 100);

    GtkWidget *down_button = gtk_button_new();
    GtkWidget *down_image = gtk_image_new_from_icon_name("go-down", GTK_ICON_SIZE_BUTTON);
    gtk_widget_set_name(down_image, "down-img");
    gtk_button_set_image(GTK_BUTTON(down_button), down_image);
    gtk_widget_set_size_request(down_button, 100, 100);

    // Optional: Add margins for spacing
    gtk_widget_set_margin_end(oxygen_button, 100);
    gtk_widget_set_margin_start(up_button, 100);

    gtk_widget_set_margin_end(co2_button, 100);
    gtk_widget_set_margin_start(down_button, 100);

    // Grid layout: Column, Row, Width, Height
    gtk_grid_attach(GTK_GRID(grid), oxygen_button, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), oxygen_reading, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), up_button,      2, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), co2_button,     0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), co2_reading,    1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), down_button,    2, 1, 1, 1);

    // Pack everything
    gtk_box_pack_start(GTK_BOX(main_vbox), welcome, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(main_vbox), grid, TRUE, TRUE, 10);

    return main_vbox;
}

