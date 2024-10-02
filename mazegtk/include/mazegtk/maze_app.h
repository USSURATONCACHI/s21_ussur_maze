#ifndef MAZEGTK_MAZE_APP_H_
#define MAZEGTK_MAZE_APP_H_

#include <gtk/gtk.h>
#include <mazegtk/viewmodel.h>
#include <mazegtk/dumb_oop.h>
#include <stdbool.h>

#define RESOURCES_ENV_VAR "S21_USSUR_MAZEGTK_RESOURCES_FILE"
#define RESOURCES_DEFAULT_FILE "/usr/share/s21_ussur_mazegtk.gresource"


typedef struct {
    PUBLIC(
        MgViewmodelState state;
    )
    PRIVATE(
        GtkApplication* app;
        GtkBuilder* builder;
        GResource* resource;

        GtkWindow* ref_main_window;
        GtkStack*  ref_main_window_stack;
        GtkWidget* ref_loading_ui;
        GtkLabel*  ref_loading_text;
        GtkWidget* ref_dropdown_ui;
        GtkWidget* ref_show_maze_ui;
    )

    METHOD(void, run, int argc, char** argv);

    METHOD(bool, render_gl, GtkGLArea* widget, GdkGLContext* context);
    METHODP(void, free);
    METHODP(void, activate);
    METHODP(void, destroy);
    METHODP(void, update_shown_state);
    
    METHODP(void, drag_moved);
    METHODP(void, drag_ended);
} MgMazeApp;

IMPL_METHOD(MgMazeApp, void, run, int argc, char** argv);
IMPL_METHOD(MgMazeApp, bool, render_gl, GtkGLArea* widget, GdkGLContext* context);
IMPL_METHODP(MgMazeApp, void, free);
IMPL_METHODP(MgMazeApp, void, activate);
IMPL_METHODP(MgMazeApp, void, destroy);
IMPL_METHODP(MgMazeApp, void, drag_moved);
IMPL_METHODP(MgMazeApp, void, drag_ended);
IMPL_METHODP(MgMazeApp, void, update_shown_state);

ST_METHOD(MgMazeApp, MgMazeApp*, create, GError** out_error);

G_MODULE_EXPORT void mg_maze_app_handle_activate(GtkWidget* widget, MgMazeApp* maze_app);
G_MODULE_EXPORT void mg_maze_app_handle_destroy(GtkWidget *widget, MgMazeApp* maze_app);

#endif // MAZEGTK_MAZE_APP_H_