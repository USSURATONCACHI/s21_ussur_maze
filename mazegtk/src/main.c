#include <mazegtk/model/camera.h>
#include <mazegtk/controller/camera.h>
#include <mazegtk/view/gtk_view.h>
#include <better_c_std/prettify.h>


int main(int argc, char** argv) {    
    // MODEL & CONTROLLEr
    MgModel model = MgModel_new(128 * 128, 128 * 128);
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