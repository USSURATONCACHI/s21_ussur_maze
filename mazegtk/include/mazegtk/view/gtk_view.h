#ifndef MAZEGTK_VIEW_GTK_VIEW_H_
#define MAZEGTK_VIEW_GTK_VIEW_H_

#include <mazegtk/util/error_list.h>

#include <gtk/gtk.h>
#include <opengl_utils/gl_program.h>
#include <opengl_utils/mesh.h>

#define RESOURCES_ENV_VAR "S21_USSUR_MAZEGTK_RESOURCES_FILE"
#define RESOURCES_DEFAULT_FILE "/usr/share/s21_ussur_mazegtk.gresource"

typedef struct {
    void* model;
    void* controller;

    GtkApplication* app;
    GtkBuilder* builder;
    GResource* resource;

    GlProgram   main_shader;
    Mesh        fullscreen_mesh;

    struct {
        vec_GError_ptr* create_errors_list;
        bool* is_activate_done;
    } waits;
} MgGtkView;


MgGtkView* MgGtkView_create_sync(void* model, void* controller, int argc, char** argv, GError** out_error);
void MgGtkView_free_sync(MgGtkView* view);

#endif // MAZEGTK_VIEW_GTK_VIEW_H_