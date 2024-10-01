#include "gtk/gtk.h"
#include <stdlib.h>
#include <stdbool.h>

#include <glib.h>
#include <gio/gio.h>
#include <mazegtk/maze_app.h>
#include <mazegtk/domain.h>

#define PUTERR(out_error) if (out_error) (*(out_error))

static bool register_resource(GError** out_error);

MazeApp* maze_app_new(GError** out_error) {
    if (!register_resource(out_error))
        return NULL;
    
    MazeApp* maze_app = (MazeApp*) malloc(sizeof(MazeApp));
    if (maze_app == NULL) {
        PUTERR(out_error) = g_error_new(DOMAIN, 0, "failed to allocated memory for MazeApp.");
        return NULL;
    }

    GtkApplication* app = gtk_application_new("org.ussur.mazegtk", G_APPLICATION_DEFAULT_FLAGS);
    if (app == NULL) {
        PUTERR(out_error) = g_error_new_literal(DOMAIN, 0, "failed to create a GTK app.");
        free(maze_app);
        return NULL;
    }
    maze_app->app = app;
    g_signal_connect(app, "startup", G_CALLBACK(maze_app_startup), maze_app);
    g_signal_connect(app, "activate", G_CALLBACK(maze_app_activate), maze_app);
    g_application_set_resource_base_path(G_APPLICATION (app), "/org/ussur/mazegtk/");


    GtkBuilder* builder = gtk_builder_new_from_resource("/org/ussur/mazegtk/main.glade");
    if (builder == NULL) {
        PUTERR(out_error) = g_error_new_literal(DOMAIN, 0, "failed to create a builder from resource.");
        g_object_unref(app);
        free(maze_app);
        return NULL;
    }
    maze_app->builder = builder;
    return maze_app;
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

    gtk_window_set_application(GTK_WINDOW(window), app->app);
    gtk_widget_show_all(GTK_WIDGET(window));

    GtkStack* main_window_stack = GTK_STACK(gtk_builder_get_object(app->builder, "main_window_stack"));
    GtkWidget* main_ui = GTK_WIDGET(gtk_builder_get_object(app->builder, "main_ui"));

    gtk_stack_set_visible_child(main_window_stack, main_ui);
    
}

void maze_app_destroy(GtkWidget *widget, MazeApp* data) {
    g_info("Destroying the Maze App");
    g_application_quit(G_APPLICATION(data->app));
}


static bool register_resource(GError** out_error) {
   const char* res_file = getenv(RESOURCES_ENV_VAR);

   if (res_file == NULL)
      res_file = RESOURCES_DEFAULT_FILE;

   GResource* resource = g_resource_load(res_file, out_error);

   if (resource == NULL) {
      return false;
   } else {
      g_message("Loaded resource from '%s'", res_file);
      g_resources_register(resource);
      return true;
   }
}

G_MODULE_EXPORT void handle_skibidi_clicked(GtkWidget* widget, MazeApp* app) {
   g_info("I am so sigma");

    GtkStack* main_window_stack = GTK_STACK(gtk_builder_get_object(app->builder, "main_window_stack"));
    GtkWidget* ui = GTK_WIDGET(gtk_builder_get_object(app->builder, "launching_ui"));

    gtk_stack_set_visible_child(main_window_stack, ui);
}