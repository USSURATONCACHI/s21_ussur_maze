#include <better_c_std/camera/camera.h>
#include <mazegtk/controller/camera.h>
#include <mazegtk/view_gtk/gtk_view.h>
#include <better_c_std/prettify.h>


int main(int argc, char** argv) {    
    // MODEL & CONTROLLER
    // MgModel model = MgModel_new(20000, 20000) // Eller's algorithm is not very parallelizable, so this will take some time
    MgModel model = MgModel_new(10, 10);
    MgController* controller = MgController_new(&model);

    // VIEW
    debugln("Creating view...");
    
    gtk_init(&argc, &argv);
    MgGtkViewResult result = MgGtkView_create(controller, (MgDataForGtkLib){ argc, argv });
    if (!result.is_ok) {
        debugln("Failed to start due to error [%s][%d]: %s", g_quark_to_string(result.error->domain), result.error->code, result.error->message);
        return 1;
    }

    MgGtkView* view = result.ok;

    // Just wait while it runs
    while (MgGtkView_is_fine(view))
        usleep(50 * 1000); // sleep for 50 ms

    if (view->failed_error)
        debugln("Application failed with error:\n[%s][%d] %s", g_quark_to_string(view->failed_error->domain), view->failed_error->code, view->failed_error->message);

    MgGtkView_free(view);
    MgController_free(controller);
    MgModel_free(model);
    return 0;
}