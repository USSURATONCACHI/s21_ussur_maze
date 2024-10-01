#include "gdk/gdk.h"
#include "glib-object.h"
#include "glibconfig.h"
#include "gtk/gtk.h"
#include <stdlib.h>
#include <stdbool.h>

#include <glib.h>
#include <gio/gio.h>
#include <mazegtk/maze_app.h>
#include <mazegtk/domain.h>

#define PUTERR(out_error) if (out_error) (*(out_error))
#define CLSERR(out_error) if (out_error) { g_error_free(*out_error); *out_error = NULL; }

static GResource* register_resource(GError** out_error);
static GtkApplication* create_gtk_app(GError** out_error, gpointer callbacks_user_data);
static GtkBuilder* create_gtk_builder(GError** out_error);


// Allocate
    // Register resource
        // Create GtkApplication
            // Create GtkBuilder
                // Init references
            // Delete GtkBuilder
        // Delete GtkApplication
    // Unregister resource
// Deallocate

MazeApp* maze_app_new(GError** out_error) {
    GError* error = NULL;
    MazeApp* maze_app = NULL;


    // Allocate
    maze_app = (MazeApp*) malloc(sizeof(MazeApp));
    if (maze_app == NULL) {
        PUTERR(out_error) = g_error_new(DOMAIN, 0, "failed to allocated memory for MazeApp.");
    }
    if (!error) {
        // Register resource
        GResource* resource = register_resource(&error);

        if (!error) {
            // Create GtkApplication
            GtkApplication* app = create_gtk_app(out_error, maze_app);
            maze_app->app = app;

            if (!error) {
                // Create builder
                GtkBuilder* builder = create_gtk_builder(out_error);
                maze_app->builder = builder;

                if (!error) {
                    // Collect references and set initial UI state
                    maze_app->current_ui = MAZE_APP_UI_LAUNCHING;
                }
                if (error) {
                    // Delete builder
                    g_object_unref(builder);
                }
            }
            if (error) {
                // Delete GtkApplication
                g_object_unref(app);
            }
        }
        if (error) {
            // Unregister resource
            g_resources_unregister(resource);
        }
    }
    if (error) {
        // Unallocate
        free(maze_app);
    }

    return maze_app;
}

static GtkApplication* create_gtk_app(GError** out_error, gpointer callbacks_user_data) {
    GtkApplication* app = gtk_application_new("org.ussur.mazegtk", G_APPLICATION_DEFAULT_FLAGS);

    if (app == NULL) {
        PUTERR(out_error) = g_error_new_literal(DOMAIN, 0, "failed to create a GTK app.");
    } else {
        g_application_set_resource_base_path(G_APPLICATION (app), "/org/ussur/mazegtk/");
        g_signal_connect(app, "startup", G_CALLBACK(maze_app_startup), callbacks_user_data);
        g_signal_connect(app, "activate", G_CALLBACK(maze_app_activate), callbacks_user_data);
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

void maze_app_free(MazeApp* maze_app) {
    if (maze_app) {
        g_object_unref(G_OBJECT(maze_app->app));
        g_object_unref(G_OBJECT(maze_app->builder));
        free(maze_app);
    }
}

void maze_app_startup(GtkWidget *widget, MazeApp* app) {

}



void maze_app_activate(GtkWidget *widget, MazeApp* app) {
    gtk_builder_connect_signals(app->builder, app);

    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window"));
    if (window == NULL) {
        g_error("No main_window object found in UI layout");
        maze_app_destroy(widget, app);
        return;
    }

    gtk_window_set_application(window, app->app);
    gtk_widget_show_all(GTK_WIDGET(window));

    GtkStack* main_window_stack = GTK_STACK(gtk_builder_get_object(app->builder, "main_window_stack"));
    GtkWidget* main_ui = GTK_WIDGET(gtk_builder_get_object(app->builder, "main_ui"));

    gtk_stack_set_visible_child(main_window_stack, main_ui);

    const GtkTargetEntry target_entries[] = {
        (GtkTargetEntry) { .target = "text/uri-list", 0, 0 },
    };
    gtk_drag_dest_set(GTK_WIDGET(window), GTK_DEST_DEFAULT_ALL, target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY | GDK_ACTION_MOVE);

    
}

void maze_app_destroy(GtkWidget *widget, MazeApp* data) {
    g_info("Destroying the Maze App");
    g_application_quit(G_APPLICATION(data->app));
}



G_MODULE_EXPORT void handle_skibidi_clicked(GtkWidget* widget, MazeApp* app) {
   g_info("I am so sigma");

    GtkStack* main_window_stack = GTK_STACK(gtk_builder_get_object(app->builder, "main_window_stack"));
    GtkWidget* ui = GTK_WIDGET(gtk_builder_get_object(app->builder, "launching_ui"));

    gtk_stack_set_visible_child(main_window_stack, ui);
}


// G_MODULE_EXPORT void maze_app_drag_begin        (GtkWidget *widget, MazeApp* data) { g_info("drag_begin");        }
// G_MODULE_EXPORT void maze_app_drag_end          (GtkWidget *widget, MazeApp* data) { g_info("drag_end");          }

G_MODULE_EXPORT void maze_app_drag_data_delete  (GtkWidget *widget, MazeApp* data) { g_info("drag_data_delete");  }
G_MODULE_EXPORT void maze_app_drag_data_get     (GtkWidget *widget, MazeApp* data) { g_info("drag_data_get");     }
G_MODULE_EXPORT void maze_app_drag_data_received(GtkWidget *widget, MazeApp* data) { g_info("drag_data_received");}
G_MODULE_EXPORT void maze_app_drag_drop         (GtkWidget *widget, MazeApp* data) { g_info("drag_drop");         }
G_MODULE_EXPORT void maze_app_drag_failed       (GtkWidget *widget, MazeApp* data) { g_info("drag_failed");       }
G_MODULE_EXPORT void maze_app_drag_leave        (GtkWidget *widget, MazeApp* data) { g_info("drag_leave");        }
G_MODULE_EXPORT void maze_app_drag_motion       (GtkWidget *widget, MazeApp* data) { g_info("drag_motion");       }