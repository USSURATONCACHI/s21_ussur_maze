#ifndef MAZEGTK_MAZE_APP_H_
#define MAZEGTK_MAZE_APP_H_

#include <gtk/gtk.h>
#include <mazegtk/viewmodel.h>
#include <mazegtk/dumb_oop.h>

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
        GtkStack* ref_main_window_stack;
        GtkWidget* ref_loading_ui;
        GtkLabel* ref_loading_text;
        GtkWidget* ref_dropdown_ui;
        GtkWidget* ref_show_maze_ui;
    )

    METHOD(void, run, int argc, char** argv);
    METHODP(void, free);
    METHODP(void, activate);
    METHODP(void, destroy);
} MgMazeApp;

IMPL_METHOD(MgMazeApp, void, run, int argc, char** argv);
IMPL_METHODP(MgMazeApp, void, free);
IMPL_METHODP(MgMazeApp, void, activate);

ST_METHOD(MgMazeApp, MgMazeApp*, create);

void mg_maze_app_handle_activate(GtkWidget* widget, MgMazeApp* maze_app);
void mg_maze_app_handle_destroy(GtkWidget *widget, MgMazeApp* maze_app);

// G_MODULE_EXPORT void maze_app_drag_data_delete(GtkWidget *widget, MazeApp* data);
// G_MODULE_EXPORT void maze_app_drag_data_get(GtkWidget *widget, MazeApp* data);
// G_MODULE_EXPORT void maze_app_drag_data_received(GtkWidget *widget, MazeApp* data);
// G_MODULE_EXPORT void maze_app_drag_motion(GtkWidget *widget, MazeApp* data);

// G_MODULE_EXPORT void maze_app_drag_leave(GtkWidget *widget, MazeApp* data);
// G_MODULE_EXPORT void maze_app_drag_drop(GtkWidget *widget, MazeApp* data);
// G_MODULE_EXPORT void maze_app_drag_failed(GtkWidget *widget, MazeApp* data);

#endif // MAZEGTK_MAZE_APP_H_