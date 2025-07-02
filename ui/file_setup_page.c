#include <gtk/gtk.h>
#include "file_setup_page.h"
#include <stdio.h>
#include <string.h>

GtkWidget* create_file_setup_page() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);

    GtkWidget *label = gtk_label_new("Sensor Log");
    gtk_widget_set_name(label, "file-setup-title");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 5);

    // Create list store (columns match CSV fields)
    GtkListStore *store = gtk_list_store_new(5,
        G_TYPE_STRING,  // datetime
        G_TYPE_FLOAT,   // CO2
        G_TYPE_FLOAT,   // O2
        G_TYPE_FLOAT,   // Temp
        G_TYPE_FLOAT    // Humidity
    );

    // Open CSV and load data
    FILE *file = fopen("sensor_log.csv", "r");
    if (file) {
        char line[256];
        // Skip header
        fgets(line, sizeof(line), file);

        while (fgets(line, sizeof(line), file)) {
            char datetime[64];
            float co2, o2, temp, humidity;

            if (sscanf(line, "%63[^,],%f,%f,%f,%f", datetime, &co2, &o2, &temp, &humidity) == 5) {
                GtkTreeIter iter;
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter,
                    0, datetime,
                    1, co2,
                    2, o2,
                    3, temp,
                    4, humidity,
                    -1);
            }
        }
        fclose(file);
    } else {
        GtkWidget *error = gtk_label_new("Error: Could not open sensor_log.csv.");
        gtk_box_pack_start(GTK_BOX(box), error, FALSE, FALSE, 5);
    }

    // Set up tree view
    GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), TRUE);

    // ✅ Add grid lines between rows/columns
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree), GTK_TREE_VIEW_GRID_LINES_BOTH);

    // Create columns
    const char *titles[] = { "Date & Time", "CO₂ (%)", "O₂ (%)", "Temp (°C)", "Humidity (%)" };
    for (int i = 0; i < 5; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

        // Center-align text
        g_object_set(renderer, "xalign", 0.5, NULL);

        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            titles[i], renderer, "text", i, NULL);

        // Set even column widths
        gtk_tree_view_column_set_min_width(column, 120);
        gtk_tree_view_column_set_expand(column, TRUE);

        gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    }

    // Scrollable area
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scroll, 600, 300);
    gtk_container_add(GTK_CONTAINER(scroll), tree);
    gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 5);

    return box;
}
