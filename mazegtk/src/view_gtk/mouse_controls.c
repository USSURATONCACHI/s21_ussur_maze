#include <mazegtk/view_gtk/gtk_view.h>
#include <gtk/gtk.h>
#include <math.h>

// static void get_cursor(GtkGLArea* gl_area, gint* x, gint* y) {
//     GdkWindow* gw = gtk_widget_get_window(GTK_WIDGET(gl_area));
//     GdkDisplay *display = gdk_display_get_default();
//     GdkSeat* seat = gdk_display_get_default_seat(display);
//     GdkDevice* pointer = gdk_seat_get_pointer(seat);
//     GdkModifierType mask;
//     gdk_window_get_device_position(gw, pointer, x, y, &mask);
// }

// G_MODULE_EXPORT gboolean handle_gl_area_button_press(GtkGLArea* gl_area, GdkEventButton* event, MgGtkView* view) { 
//     unused(event);
//     MgCameraController* camera = MgController_get_camera(view->controller);
//     MgCameraController_update_anim(camera);
//     MgCameraController_on_drag_start(camera);
//     view->inner.is_dragging = true;

//     gint x, y;
//     get_cursor(gl_area, &x, &y);
//     view->inner.prev_x = x;
//     view->inner.prev_y = y;
//     gtk_widget_grab_focus(GTK_WIDGET(gl_area));

//     return FALSE; 
// }

// G_MODULE_EXPORT gboolean handle_gl_area_button_release(GtkGLArea* gl_area, GdkEventButton* event, MgGtkView* view) { 
//     unused(gl_area);
//     unused(event);
//     MgCameraController* camera = MgController_get_camera(view->controller);
//     MgCameraController_on_drag_end(camera);
//     view->inner.is_dragging = false;
//     return FALSE;
// }
// G_MODULE_EXPORT void handle_gl_area_motion(GtkGLArea* gl_area, GdkEventMotion* event, MgGtkView* view) {
//     unused(event);
//     if (view->inner.is_dragging) {
//         gint x, y;
//         get_cursor(gl_area, &x, &y);

//         int dx = x - view->inner.prev_x;
//         int dy = y - view->inner.prev_y;
//         view->inner.prev_x = x;
//         view->inner.prev_y = y;


//         MgCameraController* camera = MgController_get_camera(view->controller);
//         long double cell_size = MgCameraController_get_cell_size(camera);
//         MgCameraController_on_drag(camera, (MgVector2) { -dx / cell_size, -dy / cell_size });
//     }
// }


// #define SCROLL_SENSITIVITY 0.1

// G_MODULE_EXPORT gboolean handle_gl_area_scroll(GtkGLArea* gl_area, GdkEventScroll* event, MgGtkView* view) { 
//     unused(gl_area);
//     MgCameraController* camera = MgController_get_camera(view->controller);

//     MgCameraController_update_anim(camera);
//     switch (event->direction) {
//         case GDK_SCROLL_UP:    MgCameraController_on_zoom(camera, 10.0 * SCROLL_SENSITIVITY); break;
//         case GDK_SCROLL_RIGHT: MgCameraController_on_zoom(camera, 10.0 * SCROLL_SENSITIVITY); break;
//         case GDK_SCROLL_DOWN:  MgCameraController_on_zoom(camera, -10.0 * SCROLL_SENSITIVITY); break;
//         case GDK_SCROLL_LEFT:  MgCameraController_on_zoom(camera, -10.0 * SCROLL_SENSITIVITY); break;
//         case GDK_SCROLL_SMOOTH:
//             MgCameraController_on_zoom(camera, (event->delta_x + event->delta_y) * SCROLL_SENSITIVITY);
//             break;
//     }

//     view->inner.is_dragging = false;
//     return TRUE;
// }