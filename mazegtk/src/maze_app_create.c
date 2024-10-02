#include <mazegtk/maze_app.h>
#include <mazegtk/domain.h>
#include <libmaze/util.h>

#define PUTERR(out_error) if (out_error) (*(out_error))

static GResource* register_resource(GError** out_error);
static GtkApplication* create_gtk_app(GError** out_error, gpointer callbacks_user_data);
static GtkBuilder* create_gtk_builder(GError** out_error);

ST_METHOD(MgMazeApp, MgMazeApp*, create, GError** out_error) {
    debugln("MgMazeApp_create called");
    GError* error = NULL;

    // Allocate
    debugln("Allocating MgMazeApp...");
    MgMazeApp* maze_app = (MgMazeApp*) malloc(sizeof(MgMazeApp));
    if (maze_app == NULL) {
        debugln("Failed.");
        error = g_error_new(DOMAIN, 0, "failed to allocated memory for MgMazeApp.");
    }
    if (!error) {
        // Register resource
        debugln("Registering resource...");
        maze_app->private.resource = register_resource(&error);

        if (!error) {
            // Create GtkApplication
            debugln("Creating GTK app...");
            maze_app->private.app = create_gtk_app(&error, maze_app);

            if (!error) {
                // Create builder
                debugln("Creating GTK Builder...");
                maze_app->private.builder = create_gtk_builder(&error);

                if (!error) {
                    // Collect references and set initial UI state
                    debugln("Creating viewmodel state...");
                    maze_app->state = mg_viewmodel_state_new();
                    maze_app->private.ref_main_window = NULL;
                    maze_app->private.ref_dropdown_ui = NULL;
                    maze_app->private.ref_loading_ui = NULL;
                    maze_app->private.ref_loading_text = NULL;
                    maze_app->private.ref_show_maze_ui = NULL;
                    if (error) {
                        debugln("Unrefing GTK Builder...");
                        g_object_unref(maze_app->private.builder);
                    }
                }
                if (error) {
                    debugln("Unrefing GTK app...");
                    g_object_unref(maze_app->private.app);
                }
            }
            if (error) {
                debugln("Unregistering resource...");
                g_resources_unregister(maze_app->private.resource);
            }
        }
        if (error) {
            debugln("Freeing maze app...");
            free(maze_app);
            maze_app = NULL;
        }
    }
    if (!error) {
        debugln("Filling method pointers in maze app");
        maze_app->free               = (void*) METHOD_NAME(MgMazeApp, free);
        maze_app->activate           = (void*) METHOD_NAME(MgMazeApp, activate);
        maze_app->destroy            = (void*) METHOD_NAME(MgMazeApp, destroy);
        maze_app->drag_moved         = (void*) METHOD_NAME(MgMazeApp, drag_moved);
        maze_app->drag_ended         = (void*) METHOD_NAME(MgMazeApp, drag_ended);
        maze_app->free               = (void*) METHOD_NAME(MgMazeApp, free);
        maze_app->run                = (void*) METHOD_NAME(MgMazeApp, run);
        maze_app->update_shown_state = (void*) METHOD_NAME(MgMazeApp, update_shown_state);
        maze_app->render_gl          = (void*) METHOD_NAME(MgMazeApp, render_gl);
    }
    debugln("MgMazeApp_create done: %p", maze_app);

    if (out_error)
        *out_error = error;
    return maze_app;
}

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
    debugln("Env variable '%s' is: '%s'", RESOURCES_ENV_VAR, res_file);

    if (res_file == NULL)
        res_file = RESOURCES_DEFAULT_FILE;

    GResource* resource = g_resource_load(res_file, out_error);

    if (resource) {
        g_message("Loaded resource from '%s'", res_file);
        g_resources_register(resource);
    }

    return resource;
}
