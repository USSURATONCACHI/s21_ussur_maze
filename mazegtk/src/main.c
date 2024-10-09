#include <better_c_std/camera/camera.h>
#include <mazegtk/controller/camera.h>
#include <mazegtk/view_gtk/gtk_view.h>
#include <better_c_std/prettify.h>


int main(int argc, char** argv) {    
    // MODEL & CONTROLLER
    MgModel model = MgModel_new(10, 10);
    MgController* controller = MgController_new(&model);

    // VIEW
    debugln("Creating view...");
    
    gtk_init(&argc, &argv);
    MgGtkViewResult result = MgGtkView_create(controller, (MgDataForGtkLib){ argc, argv });
    if (!result.is_ok) {
        debugln("Failed to start due to error:\n[%s][%d]: %s", g_quark_to_string(result.error->domain), result.error->code, result.error->message);
        return 1;
    }

    MgGtkView* view = result.ok;

    // Just wait while it runs
    MgGtkView_join_thread(view);

    if (view->failed_error)
        debugln("Application failed with error:\n[%s][%d] %s", g_quark_to_string(view->failed_error->domain), view->failed_error->code, view->failed_error->message);

    MgGtkView_free(view);
    MgController_free(controller);
    MgModel_free(model);
    return 0;
}