#include "landing_page.h"

LandingPage create_landing_page(void) {
    LandingPage page;

    page.current_co2 = 3.0;
    page.current_o2 = 20.9;

    // Main vertical container
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // Create grid for two side-by-side quadrants
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);

    // CO2 button - large, expands in both directions
    page.co2_button = gtk_button_new_with_label("% Carbon Dioxide");
    gtk_widget_set_name(page.co2_button, "co2_btn");
    gtk_widget_set_hexpand(page.co2_button, TRUE);
    gtk_widget_set_vexpand(page.co2_button, TRUE);

    // O2 button - large, expands in both directions
    page.oxygen_button = gtk_button_new_with_label("% Oxygen");
    gtk_widget_set_name(page.oxygen_button, "oxygen_btn");
    gtk_widget_set_hexpand(page.oxygen_button, TRUE);
    gtk_widget_set_vexpand(page.oxygen_button, TRUE);

    // Attach buttons side-by-side
    gtk_grid_attach(GTK_GRID(grid), page.co2_button, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), page.oxygen_button, 1, 0, 1, 1);

    // Pack grid into main_vbox
    gtk_box_pack_start(GTK_BOX(main_vbox), grid, TRUE, TRUE, 0);

    page.widget = main_vbox;

    return page;
}
