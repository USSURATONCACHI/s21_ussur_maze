#ifndef MAZEGTK_VIEW_GTK_VIEW_H_
#define MAZEGTK_VIEW_GTK_VIEW_H_

#include <mazegtk/util/error_list.h>

#include <gtk/gtk.h>
#include <opengl_utils/gl_program.h>
#include <opengl_utils/mesh.h>

#define RESOURCES_ENV_VAR "S21_USSUR_MAZEGTK_RESOURCES_FILE"
#define RESOURCES_DEFAULT_FILE "/usr/share/s21_ussur_mazegtk.gresource"

typedef struct {
    // Resources to manage Gtk GL app
    GtkApplication* app;
    GtkBuilder* builder;
    GResource* resource;

    bool is_stopped;
    struct {
        vec_GError_ptr* create_errors_list;
        bool* is_activate_done;
    } waits;

    // Application-specific resources
    void* model;
    void* controller;

    GlProgram main_shader;
    Mesh      fullscreen_mesh;
    GLuint    maze_ssbo;
} MgGtkView;


MgGtkView* MgGtkView_create_sync(void* model, void* controller, int argc, char** argv, GError** out_error);
void MgGtkView_free_sync(MgGtkView* view);
bool MgGtkView_is_fine(const MgGtkView* view);

gboolean MgGtkView_startup_prepare(MgGtkView* view);

void MgGtkView_handle_destroy(void* dont_care, MgGtkView* view);

#endif // MAZEGTK_VIEW_GTK_VIEW_H_