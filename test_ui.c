#include <gtk/gtk.h>

GtkWidget *label_o2;
GtkWidget *label_co2;
GtkWidget *dropdown;
GtkWidget *stack;

int oxygen_level = 21;
int co2_level = 5;

void update_gas_labels() {
    gchar buffer[64];
    g_snprintf(buffer, sizeof(buffer), "Oxygen: %d%%", oxygen_level);
    gtk_label_set_text(GTK_LABEL(label_o2), buffer);

    g_snprintf(buffer, sizeof(buffer), "Carbon Dioxide: %d%%", co2_level);
    gtk_label_set_text(GTK_LABEL(label_co2), buffer);
}

void show_gas_popup(GtkWidget *parent, const gchar *gas, int *value) {
    GtkWidget *dialog, *content_area, *box, *up_btn, *down_btn, *label;
    gchar msg[64];

    dialog = gtk_dialog_new_with_buttons(gas, GTK_WINDOW(parent), GTK_DIALOG_MODAL, NULL);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 200);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(content_area), box);

    g_snprintf(msg, sizeof(msg), "%s Level: %d%%", gas, *value);
    label = gtk_label_new(msg);
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 5);

    up_btn = gtk_button_new_with_label("▲ Increase");
    down_btn = gtk_button_new_with_label("▼ Decrease");

    gtk_box_pack_start(GTK_BOX(box), up_btn, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(box), down_btn, TRUE, TRUE, 5);

    g_signal_connect(up_btn, "clicked", G_CALLBACK(+[](GtkWidget *btn, gpointer user_data) {
        int *val = (int *)user_data;
        if (*val < 100) (*val)++;
        gtk_widget_destroy(gtk_widget_get_toplevel(btn));
        update_gas_labels();
    }), value);

    g_signal_connect(down_btn, "clicked", G_CALLBACK(+[](GtkWidget *btn, gpointer user_data) {
        int *val = (int *)user_data;
        if (*val > 0) (*val)--;
        gtk_widget_destroy(gtk_widget_get_toplevel(btn));
        update_gas_labels();
    }), value);

    gtk_widget_show_all(dialog);
}

static void on_dropdown_changed(GtkComboBoxText *combo, gpointer user_data) {
    GtkStack *stack = GTK_STACK(user_data);
    const gchar *selected = gtk_combo_box_text_get_active_text(combo);
    gtk_stack_set_visible_child_name(stack, selected);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *main_box, *gas_box;
    GtkWidget *o2_btn, *co2_btn;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Tri-Gas Touch UI");
    gtk_window_fullscreen(GTK_WINDOW(window));

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    dropdown = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), "Main");
    gtk_combo_box_set_active(GTK_COMBO_BOX(dropdown), 0);
    gtk_box_pack_start(GTK_BOX(main_box), dropdown, FALSE, FALSE, 5);

    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_box_pack_start(GTK_BOX(main_box), stack, TRUE, TRUE, 0);

    GtkWidget *main_screen = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_stack_add_titled(GTK_STACK(stack), main_screen, "Main", "Main");

    gas_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_box_set_homogeneous(GTK_BOX(gas_box), TRUE);

    label_o2 = gtk_label_new(NULL);
    label_co2 = gtk_label_new(NULL);
    update_gas_labels();

    gtk_box_pack_start(GTK_BOX(gas_box), label_o2, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(gas_box), label_co2, FALSE, FALSE, 10);

    o2_btn = gtk_button_new_with_label("Adjust O₂");
    g_signal_connect(o2_btn, "clicked", G_CALLBACK(+[](GtkWidget *btn, gpointer user_data) {
        show_gas_popup(btn, "Oxygen", &oxygen_level);
    }), NULL);

    co2_btn = gtk_button_new_with_label("Adjust CO₂");
    g_signal_connect(co2_btn, "clicked", G_CALLBACK(+[](GtkWidget *btn, gpointer user_data) {
        show_gas_popup(btn, "Carbon Dioxide", &co2_level);
    }), NULL);

    gtk_box_pack_start(GTK_BOX(gas_box), o2_btn, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(gas_box), co2_btn, FALSE, FALSE, 10);

    gtk_box_pack_start(GTK_BOX(main_screen), gas_box, TRUE, TRUE, 10);

    g_signal_connect(dropdown, "changed", G_CALLBACK(on_dropdown_changed), stack);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.trigas.touch", G_APPLICATION_FLAGS_NONE);
    int status;
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
