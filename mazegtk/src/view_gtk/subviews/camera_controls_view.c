#include <mazegtk/view_gtk/subviews/camera_controls_view.h>
#include <mazegtk/view_gtk/subviews/ui_update_macros.h>
#include <mazegtk/util/common_macros.h>
#include <better_c_std/prettify.h>

static void h_x_changed(GtkSpinButton* widget, MgCameraControlsView* view);
static void h_y_changed(GtkSpinButton* widget, MgCameraControlsView* view);
static void h_zoom_changed(GtkSpinButton* widget, MgCameraControlsView* view);
static void h_reset_camera(GtkSpinButton* widget, MgCameraControlsView* view);

MgCameraControlsView* MgCameraControlsView_create(GtkBuilder* ui, MgCameraController* controller) {
    assert_m(ui != NULL);
    assert_m(controller != NULL);

    MgCameraControlsView* view = (void*) malloc(sizeof(MgCameraControlsView));
    assert_alloc(view);

    *view = (MgCameraControlsView) {
        .controller = controller,

        .x_btn     = GTK_SPIN_BUTTON(gtk_builder_get_object(ui, "cam_pos_x")),
        .y_btn     = GTK_SPIN_BUTTON(gtk_builder_get_object(ui, "cam_pos_y")),
        .zoom_btn  = GTK_SPIN_BUTTON(gtk_builder_get_object(ui, "cam_zoom")),
        .reset_btn = GTK_BUTTON(gtk_builder_get_object(ui, "reset_camera")),

        .last_shown = {}, // can be uninitialized, does not matter
    };

    if (view->x_btn)     g_signal_connect(view->x_btn,     "value-changed", G_CALLBACK(h_x_changed),    view);
    if (view->y_btn)     g_signal_connect(view->y_btn,     "value-changed", G_CALLBACK(h_y_changed),    view);
    if (view->zoom_btn)  g_signal_connect(view->zoom_btn,  "value-changed", G_CALLBACK(h_zoom_changed), view);
    if (view->reset_btn) g_signal_connect(view->reset_btn, "clicked",       G_CALLBACK(h_reset_camera), view);

    return view;
}

void MgCameraControlsView_free(MgCameraControlsView* view) {
    if (view == NULL)
        return;
    debugln(__PRETTY_FUNCTION__);

    if (view->x_btn)     g_signal_handlers_disconnect_by_data(view->x_btn,     view);
    if (view->y_btn)     g_signal_handlers_disconnect_by_data(view->y_btn,     view);
    if (view->zoom_btn)  g_signal_handlers_disconnect_by_data(view->zoom_btn,  view);
    if (view->reset_btn) g_signal_handlers_disconnect_by_data(view->reset_btn, view);

    free(view);
    debugln("%s done", __PRETTY_FUNCTION__);
}

void MgCameraControlsView_update_ui(MgCameraControlsView* view) {
    MgVector2 cam_pos = MgCameraController_get_pos(view->controller);
    long double cam_zoom = MgCameraController_get_zoom(view->controller);

    UPDATE_SPINBTN_GDOUBLE(view->x_btn,    view->last_shown.cam_x, cam_pos.x);
    UPDATE_SPINBTN_GDOUBLE(view->y_btn,    view->last_shown.cam_y, cam_pos.y);
    UPDATE_SPINBTN_GDOUBLE(view->zoom_btn, view->last_shown.zoom,  cam_zoom);
}

// ==
// Handlers
// ==

// X changed
static void h_x_changed(GtkSpinButton* widget, MgCameraControlsView* view) {
    CHECK_GDOUBLE_CHANGED(val, widget, view->last_shown.cam_x);
    MgCameraController_set_x(view->controller, val);
}

// Y changed
static void h_y_changed(GtkSpinButton* widget, MgCameraControlsView* view) {
    CHECK_GDOUBLE_CHANGED(val, widget, view->last_shown.cam_y);
    MgCameraController_set_y(view->controller, val);
}

// Zoom changed
static void h_zoom_changed(GtkSpinButton* widget, MgCameraControlsView* view) {
    CHECK_GDOUBLE_CHANGED(val, widget, view->last_shown.zoom);
    MgCameraController_set_zoom(view->controller, val);
}

// Reset camera
static void h_reset_camera(GtkSpinButton* widget, MgCameraControlsView* view) {
    unused(widget);
    MgCameraController_reset_camera(view->controller);
}