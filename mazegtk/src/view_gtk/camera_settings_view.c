#include <mazegtk/view_gtk/camera_settings_view.h>
#include <mazegtk/util/common_macros.h>
#include <better_c_std/prettify.h>


static void h_drag_sensitivity_changed(GtkSpinButton* widget, MgCameraSettingsView* view);
static void h_zoom_speed_changed(GtkSpinButton* widget, MgCameraSettingsView* view);
static void h_reset_default(GtkButton* widget, MgCameraSettingsView* view);

MgCameraSettingsView* MgCameraSettingsView_create(GtkBuilder* ui, MgCameraController* controller) {
    assert_m(ui != NULL);
    assert_m(controller != NULL);

    MgCameraSettingsView* view = (void*) malloc(sizeof(MgCameraSettingsView));
    assert_alloc(view);

    *view = (MgCameraSettingsView) {
        .controller = controller,

        .drag_sensitivity_btn = GTK_SPIN_BUTTON(gtk_builder_get_object(ui, "drag_sensitivity")),
        .zoom_speed_btn       = GTK_SPIN_BUTTON(gtk_builder_get_object(ui, "zoom_speed")),
        .reset_btn            = GTK_BUTTON(gtk_builder_get_object(ui, "cam_settings_reset")),

        .last_shown = {}, // can be uninitialized, does not matter 
    };

    if (view->drag_sensitivity_btn) g_signal_connect(view->drag_sensitivity_btn, "value-changed", G_CALLBACK(h_drag_sensitivity_changed), view);
    if (view->zoom_speed_btn)       g_signal_connect(view->zoom_speed_btn,       "value-changed", G_CALLBACK(h_zoom_speed_changed),       view);
    if (view->reset_btn)            g_signal_connect(view->reset_btn,            "clicked",       G_CALLBACK(h_reset_default),            view);

    return view;
}

void MgCameraSettingsView_free(MgCameraSettingsView* view) {
    if (view == NULL)
        return;

    if (view->drag_sensitivity_btn) g_signal_handlers_disconnect_by_data(view->drag_sensitivity_btn, view);
    if (view->zoom_speed_btn)       g_signal_handlers_disconnect_by_data(view->zoom_speed_btn,       view);
    if (view->reset_btn)            g_signal_handlers_disconnect_by_data(view->reset_btn,            view);

    free(view);
}


// ==
// Handlers
// ==

// Drag sensitivity changed
static void h_drag_sensitivity_changed(GtkSpinButton* widget, MgCameraSettingsView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->last_shown.drag_sensitivity)
        return;

    MgCameraController_set_drag_sens(view->controller, val);
}

// Zoom speed changed
static void h_zoom_speed_changed(GtkSpinButton* widget, MgCameraSettingsView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    debugln("%lf (against %lf)", val, view->last_shown.zoom_speed);
    if (val == view->last_shown.zoom_speed)
        return;

    debugln("Setting the zoom sensitivity");
    MgCameraController_set_zoom_sens(view->controller, val);
}

// Reset to default
static void h_reset_default(GtkButton* widget, MgCameraSettingsView* view) {
    unused(widget);
    MgCameraController_reset_settings(view->controller);
}