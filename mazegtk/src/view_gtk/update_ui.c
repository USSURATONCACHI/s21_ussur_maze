#include <gtk/gtk.h>
#include <mazegtk/view_gtk/gtk_view.h>
#include <math.h>

static void update_camera_ui(MgGtkView* view);

G_MODULE_EXPORT void mg_maze_app_handle_update_ui(GtkGLArea* gl_area, GdkGLContext* context, MgGtkView* view) {
    unused(gl_area);
    unused(context);
    update_camera_ui(view);
}


static void update_camera_ui(MgGtkView* view) {
    MgCameraControlsView_update_ui(view->camera_controls);
    MgCameraSettingsView_update_ui(view->camera_settings);
    MgMazeGenView_update_ui(view->maze_gen);
}


// G_MODULE_EXPORT void handle_msaa_coef_value_changed(GtkSpinButton* widget, MgGtkView* view) {
//     gdouble val = gtk_spin_button_get_value(widget);
//     if (val == view->inner.last_shown.msaa_coef)
//         return;
//     view->inner.msaa_coef = val;
// }