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

G_MODULE_EXPORT void mg_maze_app_handle_gl_render(GtkGLArea* gl_area, GdkGLContext* context, MgGtkView* view) {
    unused(gl_area);
    unused(context);
    MgCameraControlsView_update_ui(view->view_camera_controls);
    MgCameraSettingsView_update_ui(view->view_camera_settings);
    MgMazeGenView_update_ui(view->view_maze_gen);
    
    MgGlMazeView_render(view->view_gl_maze);
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

    MgGlMazeViewResult result = MgGlMazeView_create(
        view->builder, 
        view->resource, 
        MgController_get_maze(view->controller),
        MgController_get_camera(view->controller),
        view->store_render_settings
    );
    if (!result.is_ok)
        return (VoidResult) ERR(result.error);
    view->view_gl_maze = result.ok;

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