#include <mazegtk/view/gtk_view.h>
#include <gtk/gtk.h>

G_MODULE_EXPORT void handle_drag_sensitivity_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.drag_sensitivity)
        return;
    view->inner.drag_sensitivity = val;
}

G_MODULE_EXPORT void handle_zoom_speed_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.cam_y)
        return;
    view->inner.zoom_speed = val;
}

G_MODULE_EXPORT void cam_controls_reset_to_default(GtkButton* widget, MgGtkView* view) {
    view->inner.zoom_speed = 1.25;
    view->inner.drag_sensitivity = 1.0;
}