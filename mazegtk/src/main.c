#include <mazegtk/model/camera.h>
#include <mazegtk/controller/camera.h>
#include <mazegtk/view/gtk_view.h>
#include <better_c_std/prettify.h>


int main(int argc, char** argv) {    
    gtk_init(&argc, &argv);

    MgModel model = {
        .maze = mz_maze_create(50, 50),
        .camera = MgCameraModel_new(0.0, 0.0),
    };
    MgController* controller = MgController_new(&model);

    debugln("Creating view...");
    GError* error = NULL;
    MgGtkView* view = MgGtkView_create_sync(controller, argc, argv, &error);
    if (error) {
        debugln("Failed to create app due to error [%s][%d]: %s", g_quark_to_string(error->domain), error->code, error->message);
        MgGtkView_free_sync(view);
        return 1;
    }

    gdk_threads_add_idle((void*)MgGtkView_startup_prepare, view);
    while (MgGtkView_is_fine(view)) {
        usleep(50 * 1000);
    }

    MgGtkView_free_sync(view);
    return 0;
}