#ifndef MAZEGTK_VIEW_GTK_VIEW_H_
#define MAZEGTK_VIEW_GTK_VIEW_H_

#include <gtk/gtk.h>
#include <better_c_std/result.h>
#include <mazegtk/controller/controller.h>
#include <mazegtk/view_gtk/gtk_view_inner.h>

#include <mazegtk/view_gtk/subviews/camera_controls_view.h>
#include <mazegtk/view_gtk/subviews/camera_settings_view.h>
#include <mazegtk/view_gtk/subviews/dropdown_view.h>
#include <mazegtk/view_gtk/subviews/grabby_cursor_view.h>
#include <mazegtk/view_gtk/subviews/maze_gen_view.h>


#define RESOURCES_ENV_VAR "S21_USSUR_MAZEGTK_RESOURCES_FILE"
#define RESOURCES_DEFAULT_FILE "/usr/share/s21_ussur_mazegtk.gresource"

typedef struct {
    // Resources to manage Gtk GL app
    GtkApplication* app;
    GtkBuilder* builder;
    GResource* resource;
    GError* failed_error;
    pthread_t thread;
    bool is_thread_running;

    // Application-specific resources
    MgCameraControlsView* camera_controls;
    MgCameraSettingsView* camera_settings;
    MgDropdownView*       dropdown;
    MgGrabbyCursorView*   grabby_cursor_view;
    MgMazeGenView*        maze_gen;

    MgController* controller;
    MgGtkViewInner inner;
} MgGtkView;

typedef STRUCT_RESULT(MgGtkView*, GError*) MgGtkViewResult;

typedef struct {
    int argc;
    char** argv;
} MgDataForGtkLib;

MgGtkViewResult MgGtkView_create(MgController* controller, MgDataForGtkLib gdata);
void MgGtkView_free(MgGtkView* view);

void MgGtkView_fail_with_error(MgGtkView* view, GError* error);
void MgGtkView_show_error_screen(MgGtkView* view, GError* error);
bool MgGtkView_is_fine(const MgGtkView* view);

G_MODULE_EXPORT void mg_maze_app_handle_update_ui(GtkGLArea* gl_area, GdkGLContext* context, MgGtkView* view);

#endif // MAZEGTK_VIEW_GTK_VIEW_H_