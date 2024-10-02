#include <glib.h>
#include <gtk/gtk.h>
#include <mazegtk/maze_app.h>
#include <better_c_std/prettify.h>


int main(int argc, char** argv) {
    debugln("Program starts");
    gtk_init(&argc, &argv);
    debugln("gtk_init done");
    
    GError* error = NULL;
    MgMazeApp* maze_app = MgMazeApp_create(&error);
    debugln("MgMazeApp_create done");
    if (maze_app == NULL) {
        if (error)
            g_error("Failed to create the Maze App [%d]: %s", error->code, error->message);
        else
            g_error("Failed to create the Maze App (unknown error).");
        return 1;
    }
    debugln("Maze app created.");

    debugln("Running...");
    MgMazeApp_run(maze_app, argc, argv);
    debugln("Run ended, freeing");

    MgMazeApp_free(maze_app);
    debugln("Maze app destroyed");
    return 0;
}