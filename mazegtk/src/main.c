#include "glib-object.h"
#include "glib.h"
#include <stdio.h>
#include <stdlib.h>

#include <gio/gio.h>
#include <gtk/gtk.h>

#define RESOURCES_ENV_VAR "S21_USSUR_MAZEGTK_RESOURCES_FILE"
#define RESOURCES_DEFAULT_FILE "/usr/share/s21_ussur_mazegtk.gresource"


void my_callback_function(GtkWidget *widget, gpointer data);
void destroy(GtkWidget *widget, gpointer data);
void greet(GtkWidget *widget, gpointer data);
static int counter = 0; 

int main(int argc, char** argv) {
   gtk_init(&argc, &argv);

   const char* res_file = getenv(RESOURCES_ENV_VAR);
   if (res_file) {
      GError* error;
      GResource* resource = g_resource_load(res_file, &error);

      if (resource == NULL) {
         g_error("Failed to load resources [%d]: %s", error->code, error->message);
         g_error_free(error);
      } else {
         g_message("Loaded resource from '%s'", res_file);
         g_resources_register(resource);
      }
   }

   GtkApplication* app = gtk_application_new("org.ussur.mazegtk", G_APPLICATION_DEFAULT_FLAGS);
   if (app == NULL) {
      g_error("Failed to create GTK application.");
      return 1;
   }
   g_debug("App created");
   g_application_set_resource_base_path(G_APPLICATION(app), "/org/ussur/mazegtk");
   
   GtkBuilder* builder = gtk_builder_new_from_resource("/org/ussur/mazegtk/main.glade");

   GtkWidget* window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
   g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
   gtk_widget_show_all(window);
   g_object_unref(GTK_WIDGET(window));

   gtk_main();

   g_object_unref(G_OBJECT(builder));
   g_object_unref(G_OBJECT(app));
   g_debug("App destroyed");
   return 0;
}

void destroy(GtkWidget *widget, gpointer) {
   gtk_main_quit ();
}
