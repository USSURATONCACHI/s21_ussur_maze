#ifndef MAZEGTK_VIEW_GTK_VIEW_H_
#define MAZEGTK_VIEW_GTK_VIEW_H_

#include <mazegtk/controller/controller.h>
#include <mazegtk/view/gtk_view_inner.h>

#include <gtk/gtk.h>

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
    MgController* controller;
    MgGtkViewInner inner;
} MgGtkView;

typedef struct {
    bool is_ok;
    union {
        MgGtkView* ok;
        GError* error;
    };
} MgGtkViewResult;

typedef struct {
    int argc;
    char** argv;
} MgDataForGtkLib;


MgGtkViewResult MgGtkView_create(MgController* controller, MgDataForGtkLib gdata);
void MgGtkView_free(MgGtkView* view);

void MgGtkView_fail_with_error(MgGtkView* view, GError* error);
bool MgGtkView_is_fine(const MgGtkView* view);

// gboolean MgGtkView_startup_prepare(MgGtkView* view);
// void MgGtkView_handle_destroy(void* dont_care, MgGtkView* view);

#endif // MAZEGTK_VIEW_GTK_VIEW_H_