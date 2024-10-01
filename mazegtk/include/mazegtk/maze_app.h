#ifndef MAZEGTK_MAZE_APP_H_
#define MAZEGTK_MAZE_APP_H_

#include <gtk/gtk.h>

#define RESOURCES_ENV_VAR "S21_USSUR_MAZEGTK_RESOURCES_FILE"
#define RESOURCES_DEFAULT_FILE "/usr/share/s21_ussur_mazegtk.gresource"

typedef enum {
   MAZE_APP_UI_LAUNCHING,
   MAZE_APP_UI_DROPDOWN,
   MAZE_APP_UI_MAIN
} MazeAppUi;

typedef struct {
   GtkWindow* main_window;

   GtkWidget* main_ui;
   GtkWidget* launching_ui;
   GtkWidget* dropdown_ui;
} MazeAppReferences;

typedef struct {
   GtkApplication* app;
   GtkBuilder* builder;

   MazeAppReferences references;
   MazeAppUi current_ui;
} MazeApp;

MazeApp* maze_app_new(GError** out_error);
void maze_app_free(MazeApp* maze_app);

void maze_app_switch_to_ui(MazeApp* app, MazeAppUi ui);

void maze_app_startup(GtkWidget *widget, MazeApp* app);
void maze_app_activate(GtkWidget *widget, MazeApp* app);
void maze_app_destroy(GtkWidget *widget, MazeApp* data);

G_MODULE_EXPORT void maze_app_drag_begin(GtkWidget *widget, MazeApp* data);
G_MODULE_EXPORT void maze_app_drag_data_delete(GtkWidget *widget, MazeApp* data);
G_MODULE_EXPORT void maze_app_drag_data_get(GtkWidget *widget, MazeApp* data);
G_MODULE_EXPORT void maze_app_drag_data_received(GtkWidget *widget, MazeApp* data);
G_MODULE_EXPORT void maze_app_drag_end(GtkWidget *widget, MazeApp* data);
G_MODULE_EXPORT void maze_app_drag_motion(GtkWidget *widget, MazeApp* data);

G_MODULE_EXPORT void maze_app_drag_leave(GtkWidget *widget, MazeApp* data);
G_MODULE_EXPORT void maze_app_drag_drop(GtkWidget *widget, MazeApp* data);
G_MODULE_EXPORT void maze_app_drag_failed(GtkWidget *widget, MazeApp* data);

#endif // MAZEGTK_MAZE_APP_H_