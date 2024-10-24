#include <mazegtk/view_gtk/subviews/camera_mouse_view.h>
#include <better_c_std/prettify.h>

static void get_cursor(GtkGLArea* gl_area, gint* x, gint* y);
static gboolean h_button_press(GtkGLArea* widget, GdkEventButton* event, MgCameraMouseView* view);
static gboolean h_button_release(GtkGLArea* widget, GdkEventButton* event, MgCameraMouseView* view);
static gboolean h_scroll(GtkGLArea* widget, GdkEventScroll* event, MgCameraMouseView* view);
static gboolean h_motion(GtkGLArea* widget, GdkEventMotion* event, MgCameraMouseView* view);

MgCameraMouseView* MgCameraMouseView_create(GtkBuilder* ui, MgCameraController* controller) {
    if (ui == NULL || controller == NULL)
        return NULL;

    MgCameraMouseView* view = (void*) malloc(sizeof(MgCameraMouseView));
    assert_alloc(view);

    *view = (MgCameraMouseView) {
        .controller = controller,
        .gl_area      = GTK_GL_AREA(gtk_builder_get_object(ui, "gl_area")),
        .context_menu = GTK_MENU(gtk_builder_get_object(ui, "cell_menu")),

        .lmb_pressed_timestamp = {},
        .is_lmb_pressed = false,
    };

    debugln(__PRETTY_FUNCTION__);
    if (view->gl_area) {
        debugln("Connecting mouse signals");
        g_signal_connect(view->gl_area, "button-press-event",   G_CALLBACK(h_button_press),   view);
        g_signal_connect(view->gl_area, "button-release-event", G_CALLBACK(h_button_release), view);
        g_signal_connect(view->gl_area, "motion-notify-event",  G_CALLBACK(h_motion),         view);
        g_signal_connect(view->gl_area, "scroll-event",         G_CALLBACK(h_scroll),         view);
    
        gtk_widget_add_events(GTK_WIDGET(view->gl_area), 
            gtk_widget_get_events(GTK_WIDGET(view->gl_area)) | 
            GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | 
            GDK_SCROLL_MASK | GDK_FOCUS_CHANGE_MASK
            
        );
    }

    if (view->context_menu) {
        GtkWidget* item_from = gtk_menu_item_new_with_label("Path from here");
        GtkWidget* item_to = gtk_menu_item_new_with_label("Path to here");

        gtk_menu_shell_append(GTK_MENU_SHELL(view->context_menu), item_from);
        gtk_menu_shell_append(GTK_MENU_SHELL(view->context_menu), item_to);
        
        gtk_widget_show_all(GTK_WIDGET(view->context_menu));
    }

    return view;
}

void MgCameraMouseView_free(MgCameraMouseView* view) {
    if (view == NULL)
        return;

    if (view->gl_area)
        g_signal_handlers_disconnect_by_data(view->gl_area, view);
    free(view);
}

static void get_cursor(GtkGLArea* gl_area, gint* x, gint* y) {
    GdkWindow*  gw      = gtk_widget_get_window(GTK_WIDGET(gl_area));
    GdkDisplay *display = gdk_display_get_default();
    GdkSeat*    seat    = gdk_display_get_default_seat(display);
    GdkDevice*  pointer = gdk_seat_get_pointer(seat);
    GdkModifierType mask;
    gdk_window_get_device_position(gw, pointer, x, y, &mask);
}
static gboolean h_button_press(GtkGLArea* widget, GdkEventButton* event, MgCameraMouseView* view) {
    unused(event); // event fields `x` and `y` seem to not carry anything except zeroes, so useless

    view->is_lmb_pressed = true;
    view->lmb_pressed_timestamp = BcstdTime_now_unix();

    MgCameraController_on_drag_start(view->controller);
    gint x, y;
    get_cursor(widget, &x, &y);
    view->prev_mouse_x = x;
    view->prev_mouse_y = y;
    gtk_widget_grab_focus(GTK_WIDGET(widget));

    return TRUE; // handled
}
static gboolean h_button_release(GtkGLArea* widget, GdkEventButton* event, MgCameraMouseView* view) {
    unused(widget);
    unused(event);

    if (view->is_lmb_pressed) {
        MgCameraController_on_drag_end(view->controller);
        view->is_lmb_pressed = false;
    }

    BcstdTime diff = BcstdTime_diff(BcstdTime_now_unix(), view->lmb_pressed_timestamp);
    bool has_clicked = BcstdTime_is_less(diff, BcstdTime_from_millis(200)); // diff < 200ms
    if (has_clicked) {
        MgCameraController_set_vel(view->controller, (MgVector2) { 0.0, 0.0 });
        gtk_menu_popup_at_pointer(view->context_menu, (GdkEvent*)event);
    }

    return TRUE; // handled
}

static gboolean h_scroll(GtkGLArea* widget, GdkEventScroll* event, MgCameraMouseView* view) {
    unused(widget);

    float zoom_delta = 0.0;
    switch (event->direction) {
        case GDK_SCROLL_UP:    zoom_delta = 1.0;  break;
        case GDK_SCROLL_RIGHT: zoom_delta = 1.0;  break;
        case GDK_SCROLL_DOWN:  zoom_delta = -1.0; break;
        case GDK_SCROLL_LEFT:  zoom_delta = -1.0; break;
        case GDK_SCROLL_SMOOTH: 
            zoom_delta = event->delta_x + event->delta_y; 
            break;
            break;
    }
    MgCameraController_on_zoom(view->controller, zoom_delta );

    return TRUE; // handled
}


static gboolean h_motion(GtkGLArea* widget, GdkEventMotion* event, MgCameraMouseView* view) {
    unused(event);
    if (view->is_lmb_pressed) {
        gint x, y;
        get_cursor(widget, &x, &y);

        int dx = x - view->prev_mouse_x;
        int dy = y - view->prev_mouse_y;
        view->prev_mouse_x = x;
        view->prev_mouse_y = y;

        long double cell_size = MgCameraController_get_cell_size(view->controller);
        MgCameraController_on_drag(
            view->controller, 
            (MgVector2) { -dx / cell_size, -dy / cell_size }
        );
    }

    return TRUE; // handled
}