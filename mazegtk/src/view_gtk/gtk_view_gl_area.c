#include <mazegtk/view_gtk/gtk_view.h>
#include <mazegtk/view_gtk/subviews/gl_maze_view.h>
#include <mazegtk/util/common_macros.h>

#include <better_c_std/result.h>
#include <better_c_std/prettify.h>

typedef struct {} Void;
typedef STRUCT_RESULT(Void, GError*) VoidResult;

static VoidResult realize(GtkGLArea* widget, MgGtkView* view);
static VoidResult unrealize(GtkGLArea* widget, MgGtkView* view);

G_MODULE_EXPORT void mg_maze_app_handle_gl_realize(GtkGLArea* widget, MgGtkView* view) {
    VoidResult result = realize(widget, view);
    if (!result.is_ok) {
        MgGtkView_fail_with_error(view, result.error);
        return;
    }
}
G_MODULE_EXPORT void mg_maze_app_handle_gl_unrealize(GtkGLArea* widget, MgGtkView* view) {
    VoidResult result = unrealize(widget, view);
    if (!result.is_ok) {
        MgGtkView_fail_with_error(view, result.error);
        return;
    }
}

// Realize
gboolean on_timeout(gpointer data) {
    GtkGLArea* gl_area = GTK_GL_AREA(data);
    gtk_gl_area_queue_render(gl_area);
    return G_SOURCE_CONTINUE; // Continue the timeout
}

static VoidResult realize(GtkGLArea* widget, MgGtkView* view) {
    gtk_gl_area_make_current(widget);

    MgGlMazeViewResult result = MgGlMazeView_create(view->builder, view->resource, MgController_get_maze(view->controller));
    if (!result.is_ok)
        return (VoidResult) ERR(result.error);
    view->view_gl_maze = result.ok;

    // Allow mouse events on GLArea
    // gtk_widget_add_events(
    //     GTK_WIDGET(widget),
    //     GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK |
    //     GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK |
    //     GDK_BUTTON_RELEASE_MASK | GDK_SCROLL_MASK |
    //     GDK_FOCUS_CHANGE_MASK
    // );

    g_timeout_add(1000 / 60, on_timeout, widget);
    return (VoidResult) OK({});
}

// Unrealize
static VoidResult unrealize(GtkGLArea* widget, MgGtkView* view) {
    gtk_gl_area_make_current(widget);
    MgGlMazeView_free(view->view_gl_maze);
    view->view_gl_maze = NULL;
    return (VoidResult) OK({});
}