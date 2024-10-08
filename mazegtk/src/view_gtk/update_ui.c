#include <gtk/gtk.h>
#include <mazegtk/view_gtk/gtk_view.h>
#include <math.h>

static void update_camera_ui(MgGtkView* view);

G_MODULE_EXPORT void mg_maze_app_handle_update_ui(GtkGLArea* gl_area, GdkGLContext* context, MgGtkView* view) {
    unused(gl_area);
    unused(context);
    update_camera_ui(view);
}


#define UPDATE_GDOUBLE(builder, name, last_shown_var, new_value) \
    {                                                                                             \
        GtkSpinButton* updating_btn = GTK_SPIN_BUTTON(gtk_builder_get_object((builder), (name))); \
        if (updating_btn && !gtk_widget_is_focus(GTK_WIDGET(updating_btn))) {                     \
            (last_shown_var) = (new_value);                                                       \
            gtk_spin_button_set_value(updating_btn, (last_shown_var));                            \
        }                                                                                         \
    }                                                                                             \
    //;

static void update_camera_ui(MgGtkView* view) {
    MgCameraController* camera = MgController_get_camera(view->controller);
    MgVector2 cam_pos = MgCameraController_get_pos(camera);
    double cam_zoom = (double)MgCameraController_get_zoom(camera);
    double drag_sens = (double)MgCameraController_get_drag_sens(camera);
    double zoom_sens = (double)MgCameraController_get_zoom_sens(camera);

    UPDATE_GDOUBLE(view->builder, "cam_pos_x", view->inner.last_shown.cam_x, cam_pos.x);
    UPDATE_GDOUBLE(view->builder, "cam_pos_y", view->inner.last_shown.cam_y, cam_pos.y);
    UPDATE_GDOUBLE(view->builder, "cam_zoom",  view->inner.last_shown.zoom, cam_zoom);

    UPDATE_GDOUBLE(view->builder, "drag_sensitivity", view->inner.last_shown.drag_sensitivity, drag_sens);
    UPDATE_GDOUBLE(view->builder, "zoom_speed", view->inner.last_shown.zoom_speed, zoom_sens);
    debugln("Setting zoom_speed to `%Lf`", zoom_sens);

    UPDATE_GDOUBLE(view->builder, "msaa_coef", view->inner.last_shown.msaa_coef, view->inner.msaa_coef);

    UPDATE_GDOUBLE(view->builder, "gen_maze_width", view->inner.last_shown.gen_maze_w, view->inner.gen_maze_w);
    UPDATE_GDOUBLE(view->builder, "gen_maze_height", view->inner.last_shown.gen_maze_h, view->inner.gen_maze_h);
}


G_MODULE_EXPORT void handle_msaa_coef_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.msaa_coef)
        return;
    view->inner.msaa_coef = val;
}
G_MODULE_EXPORT void handle_gen_maze_width_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.gen_maze_w)
        return;
    view->inner.gen_maze_w = val;
}
G_MODULE_EXPORT void handle_gen_maze_height_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.gen_maze_h)
        return;
    view->inner.gen_maze_h = val;
}
