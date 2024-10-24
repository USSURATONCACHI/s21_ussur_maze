#include <mazegtk/view_gtk/subviews/camera_keyboard_view.h>

static gboolean h_key_press(GtkWindow* widget, GdkEventKey* event, MgCameraKeyboardView* view);
static gboolean h_key_release(GtkWindow* widget, GdkEventKey* event, MgCameraKeyboardView* view);

MgCameraKeyboardView* MgCameraKeyboardView_create(GtkBuilder* ui, MgCameraController* controller) {
    if (ui == NULL || controller == NULL)
        return NULL;

    MgCameraKeyboardView* view = (void*) malloc(sizeof(MgCameraKeyboardView));
    assert_alloc(view);

    *view = (MgCameraKeyboardView) {
        .controller = controller,
        .main_window = GTK_WINDOW(gtk_builder_get_object(ui, "main_window")),

        .w = { false, false },
        .a = { false, false },
        .s = { false, false },
        .d = { false, false },
        .r = { false, false },
        .f = { false, false },
    };

    debugln(__PRETTY_FUNCTION__);
    if (view->main_window) {
        debugln("Connecting keyboard signals");
        g_signal_connect(view->main_window, "key-press-event",   G_CALLBACK(h_key_press),   view);
        g_signal_connect(view->main_window, "key-release-event", G_CALLBACK(h_key_release), view);
        gtk_widget_add_events(GTK_WIDGET(view->main_window), 
            gtk_widget_get_events(GTK_WIDGET(view->main_window)) | 
            GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK
        );
    }

    return view;
}

void MgCameraKeyboardView_free(MgCameraKeyboardView* view) {
    if (view == NULL)
        return;

    if (view->main_window)
        g_signal_handlers_disconnect_by_data(view->main_window, view);
    free(view);
}

#define HANDLE_KEY(key) if ((key).is_pressed != (key).was_pressed) (key).was_pressed = (key).is_pressed;

void MgCameraKeyboardView_update(MgCameraKeyboardView* view) {
    MgVector2 drag = {0.0, 0.0};

    bool was_any_pressed = view->w.was_pressed || view->a.was_pressed || view->s.was_pressed || view->d.was_pressed;
    bool is_any_pressed = view->w.is_pressed || view->a.is_pressed || view->s.is_pressed || view->d.is_pressed;

    // W
    HANDLE_KEY(view->w);
    HANDLE_KEY(view->s);
    HANDLE_KEY(view->a);
    HANDLE_KEY(view->d);

    if (view->w.is_pressed) drag.y -= 10.0;
    if (view->s.is_pressed) drag.y += 10.0;
    if (view->a.is_pressed) drag.x -= 10.0;
    if (view->d.is_pressed) drag.x += 10.0;

    if (!was_any_pressed && is_any_pressed) {
        MgCameraController_on_drag_start(view->controller);
    } else if (was_any_pressed && !is_any_pressed) {
        MgCameraController_on_drag_end(view->controller);
    } else if (is_any_pressed) {
        long double cell_size = MgCameraController_get_cell_size(view->controller);
        drag.x /= cell_size;
        drag.y /= cell_size;
        MgCameraController_on_drag(view->controller, drag);
    }

    if (view->r.is_pressed)
        MgCameraController_on_zoom(view->controller, 0.1);
    if (view->f.is_pressed)
        MgCameraController_on_zoom(view->controller, -0.1);
}

#undef HANDLE_WASD

#define KEYCODE_W 25
#define KEYCODE_R 27
#define KEYCODE_A 38
#define KEYCODE_S 39
#define KEYCODE_D 40
#define KEYCODE_F 41

static gboolean h_key_press(GtkWindow* widget, GdkEventKey* event, MgCameraKeyboardView* view) {
    unused(widget);
    switch (event->hardware_keycode) {
        case KEYCODE_W: view->w.is_pressed = true; break;
        case KEYCODE_A: view->a.is_pressed = true; break;
        case KEYCODE_S: view->s.is_pressed = true; break;
        case KEYCODE_D: view->d.is_pressed = true; break;
        case KEYCODE_R: view->r.is_pressed = true; break;
        case KEYCODE_F: view->f.is_pressed = true; break;

        default:
            return FALSE;  // Propagate event if it's not handled
    }

    return TRUE;  // Event handled, don't propagate further
}
static gboolean h_key_release(GtkWindow* widget, GdkEventKey* event, MgCameraKeyboardView* view) {
    unused(widget);
    switch (event->hardware_keycode) {
        case KEYCODE_W: view->w.is_pressed = false; break;
        case KEYCODE_A: view->a.is_pressed = false; break;
        case KEYCODE_S: view->s.is_pressed = false; break;
        case KEYCODE_D: view->d.is_pressed = false; break;
        case KEYCODE_R: view->r.is_pressed = false; break;
        case KEYCODE_F: view->f.is_pressed = false; break;

        default:
            return FALSE;  // Propagate event if it's not handled
    }

    return TRUE;  // Event handled, don't propagate further
}