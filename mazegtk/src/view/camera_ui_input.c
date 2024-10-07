#include <mazegtk/view/gtk_view.h>
#include <gtk/gtk.h>

G_MODULE_EXPORT void handle_cam_pos_x_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.cam_x)
        return;

    MgCameraController* camera = MgController_get_camera(view->controller);
    MgVector2 cam_pos = MgCameraController_pos(camera);
    cam_pos.x = val;
    MgCameraController_set_pos(camera, cam_pos);
}

G_MODULE_EXPORT void handle_cam_pos_y_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.cam_y)
        return;

    MgCameraController* camera = MgController_get_camera(view->controller);
    MgVector2 cam_pos = MgCameraController_pos(camera);
    cam_pos.y = val;
    MgCameraController_set_pos(camera, cam_pos);
}

G_MODULE_EXPORT void handle_cam_zoom_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.zoom)
        return;

    MgCameraController* camera = MgController_get_camera(view->controller);
    MgCameraController_set_zoom(camera, val);
}

G_MODULE_EXPORT void handle_reset_camera(GtkSpinButton* widget, MgGtkView* view) {
    unused(widget);
    MgMazeController* maze = MgController_get_maze(view->controller);
    size_t width = MgMazeController_width(maze);
    size_t height = MgMazeController_height(maze);

    MgCameraController* camera = MgController_get_camera(view->controller);
    MgCameraController_stop_movement(camera);
    MgCameraController_set_zoom(camera, 20.0);
    MgCameraController_set_pos(camera, (MgVector2) { width / 2.0f, height / 2.0f });
}