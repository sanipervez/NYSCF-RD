#ifndef LANDING_PAGE_H
#define LANDING_PAGE_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget *widget;         // Root container (main_vbox)
    GtkWidget *co2_button;     // % CO2 button
    GtkWidget *oxygen_button;  // % O2 button
    double current_co2;        // Current CO2 value
    double current_o2;         // Current O2 value
} LandingPage;

LandingPage create_landing_page(void);

#endif // LANDING_PAGE_H
