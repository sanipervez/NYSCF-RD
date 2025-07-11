#include <gtk/gtk.h>
#include "passcode_page.h"
#include <string.h>

#define PASSCODE_LENGTH 4
const char *CORRECT_PASSCODE = "1234";
char entered_passcode[PASSCODE_LENGTH + 1] = "";

GtkWidget *entry;

void check_passcode() {
    GtkWidget *dialog;

    if (strcmp(entered_passcode, CORRECT_PASSCODE) != 0) {
        dialog = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(entry)),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Access Denied"
        );
    } 

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    memset(entered_passcode, 0, sizeof(entered_passcode));
    gtk_entry_set_text(GTK_ENTRY(entry), "");
}

void on_digit_clicked(GtkButton *button, gpointer user_data) {
    const char *digit = gtk_button_get_label(button);

    if (strlen(entered_passcode) < PASSCODE_LENGTH) {
        strcat(entered_passcode, digit);
        gtk_entry_set_text(GTK_ENTRY(entry), entered_passcode);
    }
}

void on_backspace_clicked(GtkButton *button, gpointer user_data) {
    size_t len = strlen(entered_passcode);
    if (len > 0) {
        entered_passcode[len - 1] = '\0';
        gtk_entry_set_text(GTK_ENTRY(entry), entered_passcode);
    }
}

void on_enter_clicked(GtkButton *button, gpointer user_data) {
    if (strlen(entered_passcode) == PASSCODE_LENGTH) {
        check_passcode();
    } else {
        gtk_entry_set_text(GTK_ENTRY(entry), "Incomplete");
    }
}

GtkWidget* create_passcode_keypad() {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);

    // Entry field
    entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
    gtk_entry_set_max_length(GTK_ENTRY(entry), PASSCODE_LENGTH);
    gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_widget_set_vexpand(entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 3, 1);
    gtk_entry_set_alignment(GTK_ENTRY(entry), 0.5);
    gtk_widget_set_name(entry, "digits");

    int number = 1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            char label[2] = {number + '0', '\0'};
            GtkWidget *button = gtk_button_new_with_label(label);
            gtk_widget_set_name(button, "passcode-button");
            gtk_widget_set_hexpand(button, TRUE);
            gtk_widget_set_vexpand(button, TRUE);
            g_signal_connect(button, "clicked", G_CALLBACK(on_digit_clicked), NULL);
            gtk_grid_attach(GTK_GRID(grid), button, j, i + 1, 1, 1);
            number++;
        }
    }

    // Back button
    GtkWidget *back_button = gtk_button_new_with_label("Back");
    gtk_widget_set_hexpand(back_button, TRUE);
    gtk_widget_set_vexpand(back_button, TRUE);
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_backspace_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), back_button, 0, 4, 1, 1);
    gtk_widget_set_name(back_button, "passcode-button");

    // Zero button
    GtkWidget *zero_button = gtk_button_new_with_label("0");
    gtk_widget_set_hexpand(zero_button, TRUE);
    gtk_widget_set_vexpand(zero_button, TRUE);
    g_signal_connect(zero_button, "clicked", G_CALLBACK(on_digit_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), zero_button, 1, 4, 1, 1);
    gtk_widget_set_name(zero_button, "passcode-button");

    // Enter button
    GtkWidget *enter_button = gtk_button_new_with_label("Enter");
    gtk_widget_set_hexpand(enter_button, TRUE);
    gtk_widget_set_vexpand(enter_button, TRUE);
    g_signal_connect(enter_button, "clicked", G_CALLBACK(on_enter_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), enter_button, 2, 4, 1, 1);
    gtk_widget_set_name(enter_button, "passcode-button");

    return grid;
}

GtkWidget* create_passcode_page() {
    GtkWidget *outer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_hexpand(outer_box, TRUE);
    gtk_widget_set_vexpand(outer_box, TRUE);


    // Keypad
    GtkWidget *keypad = create_passcode_keypad();

    // Pack everything
    gtk_box_pack_start(GTK_BOX(outer_box), keypad, TRUE, TRUE, 10);

    return outer_box;
}
