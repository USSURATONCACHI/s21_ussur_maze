#ifndef MAZEGTK_VIEW_GTK_VIEW_H_
#define MAZEGTK_VIEW_GTK_VIEW_H_

#include <gtk/gtk.h>
#include <better_c_std/result.h>
#include <mazegtk/controller/controller.h>

#include <mazegtk/view_gtk/subviews/camera_controls_view.h>
#include <mazegtk/view_gtk/subviews/camera_settings_view.h>
#include <mazegtk/view_gtk/subviews/dropdown_view.h>
#include <mazegtk/view_gtk/subviews/grabby_cursor_view.h>
#include <mazegtk/view_gtk/subviews/maze_gen_view.h>
#include <mazegtk/view_gtk/subviews/gl_maze_view.h>
#include <mazegtk/view_gtk/subviews/camera_mouse_view.h>

#define RESOURCES_ENV_VAR "S21_USSUR_MAZEGTK_RESOURCES_FILE"
#define RESOURCES_DEFAULT_FILE "/usr/share/s21_ussur_mazegtk.gresource"

// This main `MgGtkView` just aggregates all the small subviews, and does nothing on its own.
// To add new functionality - just add more subviews in here (and call `_ui_update`s or `_renders` in render handle, if needed)
typedef struct {
    // Resources to manage Gtk GL app
    GtkApplication* app;
    GtkBuilder* builder;
    GResource* resource;

    GError* failed_error;   // We run gtk in separate thread, because glib only provides quite inflexible async callback
    pthread_t thread;       // interfaces. This really breaks the line of execution and decimate readability.
    bool is_thread_running; // This struct/class provides synchronous interface to this whole process.
                            // 
                            // GTK callbacks approach would maybe work nice, if I turned my entire program into a state-machine.
                            // However, i don't think just creating a window and rendering to it would be worth
                            // covering all parts of my code with unions, state checks, and ownership managing.

    // Application-specific functionality parts
    MgCameraControlsView* view_camera_controls;
    MgCameraSettingsView* view_camera_settings;
    MgDropdownView*       view_dropdown;
    MgGrabbyCursorView*   view_grabby_cursor;
    MgMazeGenView*        view_maze_gen;
    MgGlMazeView*         view_gl_maze;
    MgCameraMouseView*    view_camera_mouse;

    MgController* controller;
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
void MgGtkView_join_thread(const MgGtkView* view);

G_MODULE_EXPORT void mg_maze_app_handle_update_ui(GtkGLArea* gl_area, GdkGLContext* context, MgGtkView* view);

#endif // MAZEGTK_VIEW_GTK_VIEW_H_