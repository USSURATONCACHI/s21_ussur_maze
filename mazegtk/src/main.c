#include <glib.h>
#include <gtk/gtk.h>
#include <mazegtk/maze_app.h>


int main(int argc, char** argv) {
    gtk_init(&argc, &argv);
    
    GError* error = NULL;
    MgMazeApp* maze_app = MgMazeApp_create(&error);
    if (maze_app == NULL) {
        if (error)
            g_error("Failed to create the Maze App [%d]: %s", error->code, error->message);
        else
            g_error("Failed to create the Maze App (unknown error).");
        return 1;
    }
    g_info("Maze app created.");

    maze_app->run(maze_app, argc, argv);

    maze_app->free(maze_app);
    g_debug("Maze app destroyed");
    return 0;
}