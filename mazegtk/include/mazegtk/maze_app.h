#ifndef MAZEGTK_MAZE_APP_H_
#define MAZEGTK_MAZE_APP_H_

#include <gtk/gtk.h>

#define RESOURCES_ENV_VAR "S21_USSUR_MAZEGTK_RESOURCES_FILE"
#define RESOURCES_DEFAULT_FILE "/usr/share/s21_ussur_mazegtk.gresource"

typedef struct {
   GtkApplication* app;
   GtkBuilder* builder;
} MazeApp;

MazeApp* maze_app_new(GError** out_error);
void maze_app_free(MazeApp* maze_app);

void maze_app_startup(GtkWidget *widget, MazeApp* app);
void maze_app_activate(GtkWidget *widget, MazeApp* app);
void maze_app_destroy(GtkWidget *widget, MazeApp* data);

#endif // MAZEGTK_MAZE_APP_H_