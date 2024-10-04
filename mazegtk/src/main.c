#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>

#include <mazegtk/maze_app.h>
#include <better_c_std/prettify.h>
#include <libmaze/maze.h>


#include <mazegtk/model/camera.h>
#include <mazegtk/controller/camera.h>
#include <mazegtk/view/gtk_view.h>


int main(int argc, char** argv) {    
    gtk_init(&argc, &argv);

    debugln("Creating model...");
    MgCameraModel model = MgCameraModel_new(0.0, 0.0);

    debugln("Creating controller...");
    MgCameraController* controller = MgCameraController_new(&model);

    debugln("Creating view...");
    GError* error = NULL;
    MgGtkView* view = MgGtkView_create_sync(&model, &controller, argc, argv, &error);
    if (error) {
        debugln("Failed to create app due to error [%s][%d]: %s", g_quark_to_string(error->domain), error->code, error->message);
        MgGtkView_free_sync(view);
        return 1;
    }
    debugln("View created");

    g_usleep(5 * 1000000);
    
    debugln("Freeing...");
    MgGtkView_free_sync(view);
    debugln("Freeing done");
    return 0;
    // GError* error = NULL;
    // MgMazeApp* maze_app = MgMazeApp_create(&error);
    // if (maze_app == NULL) {
    //     if (error)
    //         g_error("Failed to create the Maze App [%d]: %s", error->code, error->message);
    //     else
    //         g_error("Failed to create the Maze App (unknown error).");
    //     return 1;
    // }
    // debugln("Maze app created.");

    // MgMazeApp_run(maze_app, argc, argv);

    // MgMazeApp_free(maze_app);
    // maze_free(maze);
    // debugln("Maze app destroyed");
    // return 0;
}