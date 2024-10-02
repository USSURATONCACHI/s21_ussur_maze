#include "mazegtk/viewmodel.h"
#include <mazegtk/maze_app.h>
#include <mazegtk/domain.h>

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
// typedef struct {
//    PUBLIC(
//       MgViewmodelState state;
//    )
//    PRIVATE(
//       GtkApplication* app;
//       GtkBuilder* builder;

//       GtkWindow* ref_main_window;
//       GtkWidget* ref_loading_ui;
//       GtkLabel* ref_loading_text;
//       GtkWidget* ref_dropdown_ui;
//       GtkWidget* ref_show_maze_ui;
//    )

//    METHODP(void, free);
// } MgMazeApp;
#define PUTERR(out_error) if (out_error) (*(out_error))

static GResource* register_resource(GError** out_error);
static GtkApplication* create_gtk_app(GError** out_error, gpointer callbacks_user_data);
static GtkBuilder* create_gtk_builder(GError** out_error);

ST_METHOD(MgMazeApp, MgMazeApp*, create) {
    GError* error = NULL;
    MgMazeApp* maze_app = NULL;

    // Allocate
    maze_app = (MgMazeApp*) malloc(sizeof(MgMazeApp));
    if (maze_app == NULL) {
        error = g_error_new(DOMAIN, 0, "failed to allocated memory for MgMazeApp.");
    }
    if (!error) {
        // Register resource
        maze_app->private.resource = register_resource(&error);

        if (!error) {
            // Create GtkApplication
            maze_app->private.app = create_gtk_app(&error, maze_app);

            if (!error) {
                // Create builder
                maze_app->private.builder = create_gtk_builder(&error);

                if (!error) {
                    // Collect references and set initial UI state
                    maze_app->state = mg_viewmodel_state_new();
                    maze_app->private.ref_main_window = NULL;
                    maze_app->private.ref_dropdown_ui = NULL;
                    maze_app->private.ref_loading_ui = NULL;
                    maze_app->private.ref_loading_text = NULL;
                    maze_app->private.ref_show_maze_ui = NULL;
                }
                if (error) g_object_unref(maze_app->private.builder);
            }
            if (error) g_object_unref(maze_app->private.app);
        }
        if (error) g_resources_unregister(maze_app->private.resource);
    }
    if (error) free(maze_app);
    if (!error) {
        maze_app->free = (void*) METHOD_NAME(MgMazeApp, free);
    }
    return maze_app;
}

IMPL_METHODP(MgMazeApp, void, free) {
    if (this == NULL)
        return;
    
    g_object_unref(this->private.builder);
    g_object_unref(this->private.app);
    g_resources_unregister(this->private.resource);
    free(this);
}

IMPL_METHODP(MgMazeApp, void, activate) {
    gtk_builder_connect_signals(this->private.builder, this->private.app);

    // Window
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(this->private.builder, "main_window"));
    if (window == NULL) {
        g_error("No main_window object found in UI layout");
        this->destroy(this);
        return;
    }
    const GtkTargetEntry target_entries[] = {
        (GtkTargetEntry) { .target = "text/uri-list", 0, 0 },
    };
    gtk_drag_dest_set(GTK_WIDGET(window), GTK_DEST_DEFAULT_ALL, target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY | GDK_ACTION_MOVE);

    gtk_window_set_application(window, this->private.app);
    gtk_widget_show_all(GTK_WIDGET(window));

    // Other elements
    GtkStack* main_window_stack = GTK_STACK(gtk_builder_get_object(this->private.builder, "main_window_stack"));
    GtkWidget* main_ui = GTK_WIDGET(gtk_builder_get_object(this->private.builder, "main_ui"));

    this->private.ref_main_window = window;
    this->private.ref_main_window_stack = GTK_STACK(gtk_builder_get_object(this->private.builder, "main_window_stack"));
    this->private.ref_show_maze_ui = GTK_WIDGET(gtk_builder_get_object(this->private.builder, "show_maze_ui"));
    this->private.ref_loading_text = GTK_LABEL(gtk_builder_get_object(this->private.builder, "loading_ui_text"));
    this->private.ref_dropdown_ui = GTK_WIDGET(gtk_builder_get_object(this->private.builder, "dropdown_ui"));
    this->private.ref_show_maze_ui = GTK_WIDGET(gtk_builder_get_object(this->private.builder, "show_maze_ui"));
}


IMPL_METHODP(MgMazeApp, void, destroy) {
    g_info("Destroying the Maze App");
    g_application_quit(G_APPLICATION(this->private.app));
}

IMPL_METHOD(MgMazeApp, void, run, int argc, char** argv) {
    g_application_run(G_APPLICATION(this->private.app), argc, argv);
}

void mg_maze_app_handle_activate(GtkWidget* widget, MgMazeApp* maze_app) { maze_app->activate(maze_app); }
void mg_maze_app_handle_destroy(GtkWidget *widget, MgMazeApp* maze_app)  { maze_app->destroy(maze_app); }

// Static functions
static GtkApplication* create_gtk_app(GError** out_error, gpointer callbacks_user_data) {
    GtkApplication* app = gtk_application_new("org.ussur.mazegtk", G_APPLICATION_DEFAULT_FLAGS);

    if (app == NULL) {
        PUTERR(out_error) = g_error_new_literal(DOMAIN, 0, "failed to create a GTK app.");
    } else {
        g_application_set_resource_base_path(G_APPLICATION (app), "/org/ussur/mazegtk/");
        g_signal_connect(app, "activate", G_CALLBACK(mg_maze_app_handle_activate), callbacks_user_data);
    }

    return app;
}

static GtkBuilder* create_gtk_builder(GError** out_error) {
    GtkBuilder* builder = gtk_builder_new_from_resource("/org/ussur/mazegtk/main.glade");
    if (builder == NULL) {
        PUTERR(out_error) = g_error_new_literal(DOMAIN, 0, "failed to create a builder from resource.");
    }
    return builder;
}

static GResource* register_resource(GError** out_error) {
    const char* res_file = getenv(RESOURCES_ENV_VAR);

    if (res_file == NULL)
        res_file = RESOURCES_DEFAULT_FILE;

    GResource* resource = g_resource_load(res_file, out_error);

    if (resource) {
        g_message("Loaded resource from '%s'", res_file);
        g_resources_register(resource);
    }

    return resource;
}
