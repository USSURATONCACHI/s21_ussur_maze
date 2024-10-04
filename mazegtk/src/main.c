#include <mazegtk/model/camera.h>
#include <mazegtk/controller/camera.h>
#include <mazegtk/view/gtk_view.h>
#include <better_c_std/prettify.h>


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

    while (MgGtkView_is_fine(view)) {
        usleep(50 * 1000);
    }

    debugln("Freeing...");
    MgGtkView_free_sync(view);
    debugln("Freeing done");
    return 0;
}