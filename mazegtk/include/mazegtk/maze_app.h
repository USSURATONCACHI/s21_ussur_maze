#ifndef MAZEGTK_MAZE_APP_H_
#define MAZEGTK_MAZE_APP_H_

#include <gtk/gtk.h>
#include <mazegtk/viewmodel.h>
#include <stdbool.h>

#include <opengl_utils/framebuffer.h>
#include <opengl_utils/shader.h>
#include <opengl_utils/gl_program.h>
#include <opengl_utils/mesh.h>

#define RESOURCES_ENV_VAR "S21_USSUR_MAZEGTK_RESOURCES_FILE"
#define RESOURCES_DEFAULT_FILE "/usr/share/s21_ussur_mazegtk.gresource"


typedef struct {
    MgViewmodelState state;

    struct {
        GtkApplication* app;
        GtkBuilder* builder;
        GResource* resource;

        Framebuffer read_framebuffer;
        Framebuffer write_framebuffer;
        GlProgram   main_shader;
        Mesh        fullscreen_mesh;

        GtkWindow* ref_main_window;
        GtkStack*  ref_main_window_stack;
        GtkWidget* ref_loading_ui;
        GtkLabel*  ref_loading_text;
        GtkWidget* ref_dropdown_ui;
        GtkWidget* ref_show_maze_ui;
    } private;

} MgMazeApp;

MgMazeApp* MgMazeApp_create(GError** out_error);
void MgMazeApp_free(MgMazeApp* self);

void MgMazeApp_run(MgMazeApp* self, int argc, char** argv);
bool MgMazeApp_render_gl(MgMazeApp* self, GtkGLArea* widget, GdkGLContext* context);
void MgMazeApp_activate(MgMazeApp* self);
void MgMazeApp_destroy(MgMazeApp* self);
void MgMazeApp_update_shown_state(MgMazeApp* self);
void MgMazeApp_drag_moved(MgMazeApp* self);
void MgMazeApp_drag_ended(MgMazeApp* self);

G_MODULE_EXPORT void mg_maze_app_handle_activate(GtkWidget* widget, MgMazeApp* maze_app);
G_MODULE_EXPORT void mg_maze_app_handle_destroy(GtkWidget *widget, MgMazeApp* maze_app);


#endif // MAZEGTK_MAZE_APP_H_