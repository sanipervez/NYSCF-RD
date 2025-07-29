#include <gtk/gtk.h>
#include "modal_helpers.h"

static GtkWidget *modal = NULL;
static GtkWidget *value_label = NULL;
static float *current_target = NULL;
static float temp_value = 0.0;

static void update_value_label() {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.1f", temp_value);
    gtk_label_set_text(GTK_LABEL(value_label), buffer);
}

static void on_increase(GtkButton *button, gpointer user_data) {
    temp_value += 0.1;
    update_value_label();
}

static void on_decrease(GtkButton *button, gpointer user_data) {
    temp_value -= 0.1;
    update_value_label();
}

static void on_save(GtkButton *button, gpointer user_data) {
    if (current_target) {
        *current_target = temp_value;
    }
    gtk_widget_destroy(modal);
    modal = NULL;
}

static void on_close(GtkButton *button, gpointer user_data) {
    gtk_widget_destroy(modal);
    modal = NULL;
}

void show_target_modal(GtkWidget *parent, const char *title, float *target_ptr) {
    if (modal != NULL) return;  // Prevent multiple modals

    current_target = target_ptr;
    temp_value = *target_ptr;

    modal = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_modal(GTK_WINDOW(modal), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(modal), GTK_WINDOW(gtk_widget_get_toplevel(parent)));
    gtk_window_set_title(GTK_WINDOW(modal), title);
    gtk_window_set_default_size(GTK_WINDOW(modal), 300, 300);
    gtk_window_set_position(GTK_WINDOW(modal), GTK_WIN_POS_CENTER);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 20);
    gtk_container_add(GTK_CONTAINER(modal), main_box);

    GtkWidget *title_label = gtk_label_new(title);
    gtk_widget_set_name(title_label, "modal-title");
    gtk_box_pack_start(GTK_BOX(main_box), title_label, FALSE, FALSE, 5);

    GtkWidget *dec_button = gtk_button_new_with_label("-");
    gtk_box_pack_start(GTK_BOX(main_box), dec_button, FALSE, FALSE, 5);
    g_signal_connect(dec_button, "clicked", G_CALLBACK(on_decrease), NULL);

    value_label = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(main_box), value_label, FALSE, FALSE, 5);
    update_value_label();

    GtkWidget *inc_button = gtk_button_new_with_label("+");
    gtk_box_pack_start(GTK_BOX(main_box), inc_button, FALSE, FALSE, 5);
    g_signal_connect(inc_button, "clicked", G_CALLBACK(on_increase), NULL);

    GtkWidget *save_button = gtk_button_new_with_label("Save");
    gtk_box_pack_start(GTK_BOX(main_box), save_button, FALSE, FALSE, 10);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save), NULL);

    GtkWidget *close_button = gtk_button_new_with_label("Close");
    gtk_box_pack_start(GTK_BOX(main_box), close_button, FALSE, FALSE, 5);
    g_signal_connect(close_button, "clicked", G_CALLBACK(on_close), NULL);

    gtk_widget_show_all(modal);
}
