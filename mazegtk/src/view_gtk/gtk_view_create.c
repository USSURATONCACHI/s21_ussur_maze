#include <mazegtk/view_gtk/gtk_view.h>
#include <better_c_std/result.h>
#include <mazegtk/util/common_macros.h>
#include <better_c_std/prettify.h>
#include <pthread.h>

typedef GError* GError_ptr;
#define VECTOR_ITEM_TYPE GError_ptr
#define VECTOR_ITEM_DESTRUCTOR g_error_free
#define VECTOR_IMPLEMENTATION
#define VECTOR_MAKE_STATIC
#include <better_c_std/vector.h>

// Local
typedef STRUCT_RESULT(GResource*, GError*) ResourceResult;
static ResourceResult register_resource();

typedef struct {
    MgDataForGtkLib data;
    MgGtkView* view;
} ThreadArg;
static void app_run_function(ThreadArg* arg_heap);

static void handle_destroy(void* dont_care, MgGtkView* view);
static void handle_activate(void* couldnt_care_less, MgGtkView* view);
// Local


static void* report_gerror_and_return_nullptr(GError* error, vec_GError_ptr* report_to) {
    vec_GError_ptr_push(report_to, error);
    return NULL;
}

// Oh how I miss both exceptions with destructors from C++ and question marks from Rust
// Have to make up error checking in such an ugly way
// But without this it is either a giant 300 lines if-else ladder, or even more ugly mess
#define NON_NULL_OR_GERR_REPORT(ptr, report_to, ...) (  (ptr) != NULL ? (ptr) : ((void*)report_gerror_and_return_nullptr(GERROR_NEW(__VA_ARGS__), (report_to)))  )
#define OK_OR_NULL_WITH_GERR_REPORT(result, report_to) (  (result).is_ok ? (result).ok : ((void*)report_gerror_and_return_nullptr((result).error, (report_to)))  )

MgGtkViewResult MgGtkView_create(MgController* controller, MgDataForGtkLib gdata) {
    vec_GError_ptr errors = vec_GError_ptr_create();

    // Base parts of GTK + Glade app
    GResource* resource = OK_OR_NULL_WITH_GERR_REPORT(register_resource(), &errors);
    GtkApplication* app = gtk_application_new("org.ussur.mazegtk", 0);
    GtkBuilder* builder = gtk_builder_new_from_resource("/org/ussur/mazegtk/main.glade");

    // These subviews can fail on create
    MgDropdownViewResult dropdown_res = MgDropdownView_create(builder, controller);
    MgGrabbyCursorViewResult grabby_cursor_res = MgGrabbyCursorView_create(builder, "gl_area");

    // Stores
    MgRenderSettingsStore* store_render_settings = MgRenderSettingsStore_new();

    // Aggregate
    MgGtkView* view = (void*) calloc(1, sizeof(MgGtkView));
    assert_alloc(view);
    *view = (MgGtkView) {
        .controller = controller,

        .app      = NON_NULL_OR_GERR_REPORT(app, &errors, "failed to create GtkApplication"),
        .builder  = NON_NULL_OR_GERR_REPORT(builder, &errors, "failed to create GtkBuilder"),
        .resource = resource,

        .store_render_settings = store_render_settings,

        .view_dropdown      = OK_OR_NULL_WITH_GERR_REPORT(dropdown_res, &errors),
        .view_grabby_cursor = OK_OR_NULL_WITH_GERR_REPORT(grabby_cursor_res, &errors),
        .view_gl_maze       = NULL, // We can only use GL functions after GtkGLArea::realize signal is sent. For now it will be null.

        .view_camera_controls = MgCameraControlsView_create(builder, MgController_get_camera(controller)),
        .view_camera_settings = MgCameraSettingsView_create(builder, MgController_get_camera(controller)),
        .view_maze_gen        = MgMazeGenView_create       (builder, MgController_get_maze_gen(controller)),
        .view_camera_mouse    = MgCameraMouseView_create   (builder, MgController_get_camera(controller)),
        .view_render_settings = MgRenderSettingsView_create(builder, store_render_settings),
        .view_maze_info       = MgMazeInfoView_create      (builder, MgController_get_maze(controller)),
        .view_camera_keyboard = MgCameraKeyboardView_create(builder, MgController_get_camera(controller)),

        // other fields are zeroed out by `calloc`
    };

    // Check for errors
    if (errors.length > 0) {
        for (size_t i = 0; i < errors.length; i++)
            debugln("Error [%s][%d]: %s", g_quark_to_string(errors.data[i]->domain), errors.data[i]->code, errors.data[i]->message);

        size_t errors_count = errors.length;
        vec_GError_ptr_free(errors); // GError* instances inside are cleaned up via destructor
        MgGtkView_free(view);
        return (MgGtkViewResult) ERR(GERROR_NEW("Failed to create MgGtkView due to %zu errors", errors_count));
    }

    // Run GTK app in other thread (because run function is blocking)
    g_signal_connect(view->app, "activate", G_CALLBACK(handle_activate), view);
    ThreadArg* thread_arg = (ThreadArg*)malloc(sizeof(ThreadArg));
    assert_alloc(thread_arg);
    *thread_arg = (ThreadArg) { .data = gdata, .view = view };
    pthread_create(&view->thread, NULL, (void*)app_run_function, thread_arg);

    // GTK execution will eventually jump to function `handle_activate`. It is at the bottom of this file.
    // If we're not creating a second thread - current function will just hang, until GtkApplication exits.
    // And we are unable to initialize everything BEFORE activating GtkApplication:
    //      - window can only be attached after signal "GtkApplication::activate".
    //      - destroy signal and CSS styles depend on attached window.
    //      - any opengl call will terminate the process util signal "GtkGLArea::realize", where GL context is created.
    //      - we also can only deinit OpenGL stuff on signal "GtkGLArea::unrealize", since GL context will be destroyed after it.

    view->is_thread_running = true; // Not gonna bother RWLocking that, since there will be no real multi-threading here.
                                    // This thread setup is just to not hang the program in the middle of GTK initialization.

    return (MgGtkViewResult) OK(view);
}

static ResourceResult register_resource() {
    debugln("Loading and registering resource...");

    const char* res_file = getenv(RESOURCES_ENV_VAR);
    debugln("Env variable '%s' is: '%s'", RESOURCES_ENV_VAR, res_file);

    if (res_file == NULL)
        res_file = RESOURCES_DEFAULT_FILE;

    GError* error = NULL;
    GResource* resource = g_resource_load(res_file, &error);

    if (error)
        return (ResourceResult) ERR(error);

    g_resources_register(resource);
    return (ResourceResult) OK(resource);
}


static void app_run_function(ThreadArg* arg_heap) {
    ThreadArg arg = *arg_heap;
    free(arg_heap);
    g_application_run(G_APPLICATION(arg.view->app), arg.data.argc, arg.data.argv);
    arg.view->is_thread_running = false;
}

void MgGtkView_free(MgGtkView* view) {
    debugln(__PRETTY_FUNCTION__);
    debug_push();

    MgCameraControlsView_free(view->view_camera_controls);
    MgCameraSettingsView_free(view->view_camera_settings);
    MgDropdownView_free      (view->view_dropdown       );
    MgGrabbyCursorView_free  (view->view_grabby_cursor  );
    MgMazeGenView_free       (view->view_maze_gen       );
    MgCameraMouseView_free   (view->view_camera_mouse   );
    MgMazeInfoView_free      (view->view_maze_info      );
    MgRenderSettingsView_free(view->view_render_settings);
    MgCameraKeyboardView_free(view->view_camera_keyboard);
    // MgGlMazeView can only be freed before signal "GtkGLArea::unrealize"

    MgRenderSettingsStore_free(view->store_render_settings);

    if (view->app && G_IS_APPLICATION(view->app))
        g_application_quit(G_APPLICATION(view->app));

    MgGtkView_join_thread(view);

    if (view->failed_error)
        g_error_free(view->failed_error);

    G_UNREF_SAFE(view->builder);
    G_UNREF_SAFE(view->app);
    if (view->resource)
        g_resources_unregister(view->resource); 

    free(view);

    debug_pop();
    debugln("%s done", __PRETTY_FUNCTION__);
}

void MgGtkView_fail_with_error(MgGtkView* view, GError* error) {
    if (view->app && G_IS_APPLICATION(view->app))
        g_application_quit(G_APPLICATION(view->app));

    if (view->failed_error)
        g_error_free(view->failed_error);
    view->failed_error = error;
}

bool MgGtkView_is_fine(const MgGtkView* view) {
    return view->is_thread_running;
}
void MgGtkView_join_thread(const MgGtkView* view) {
    if (view->is_thread_running) {
        pthread_join(view->thread, NULL);
    } else {
        return;
    }
}

// Activate and Destroy signals

static void handle_destroy(void* dont_care, MgGtkView* view) {
    unused(dont_care);
    debugln("handle_destroy called");

    if (G_IS_APPLICATION(view->app))
        g_application_quit(G_APPLICATION(view->app));
}

static void handle_activate(void* couldnt_care_less, MgGtkView* view) {
    unused(couldnt_care_less);
    gtk_builder_connect_signals(view->builder, view);
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(view->builder, "main_window"));
    if (window == NULL) {
        MgGtkView_fail_with_error(view, GERROR_NEW("UI does not have a `main_window` widget"));
        return;
    }

    // Apply CSS styles
    {
        GtkCssProvider *provider = gtk_css_provider_new();
        gtk_css_provider_load_from_resource(provider, "/org/ussur/mazegtk/style.css");
        GdkScreen *screen = gtk_window_get_screen(window);
        gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_object_unref(provider);
    }

    // Show window
    g_signal_connect(window, "destroy", G_CALLBACK(handle_destroy), view);
    gtk_window_set_application(window, view->app);
    gtk_widget_show_all(GTK_WIDGET(window));

    // After this, GTK execution eventually jumps to 
    // function `mg_maze_app_handle_gl_realize` in file `./gtk_view_gl_area.c`
}
