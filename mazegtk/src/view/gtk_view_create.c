#include <mazegtk/view/gtk_view.h>
#include <mazegtk/util/result.h>
#include <mazegtk/util/common_macros.h>
#include <better_c_std/prettify.h>
#include <pthread.h>

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

MgGtkViewResult MgGtkView_create(MgController* controller, MgDataForGtkLib gdata) {
    MgGtkViewResult result = OK(NULL);

    MgGtkView* view = (MgGtkView*) calloc(1, sizeof(MgGtkView));
    assert_alloc(view);
    view->controller = controller;

    // RESOURCE
    ResourceResult resource = register_resource();
    if (!resource.is_ok) {
        result = (MgGtkViewResult) ERR(resource.error);
    } else {
        view->resource = resource.ok;

        // APPLICATION
        view->app = gtk_application_new("org.ussur.mazegtk", G_APPLICATION_DEFAULT_FLAGS);
        if (view->app == NULL) {
            result = (MgGtkViewResult) ERR(GERROR_NEW("failed to create GtkApplication"));
        } else {
            g_signal_connect(view->app, "activate", G_CALLBACK(handle_activate), view);

            // BUILDER
            view->builder = gtk_builder_new_from_resource("/org/ussur/mazegtk/main.glade");
            if (view->builder == NULL) {
                result = (MgGtkViewResult) ERR(GERROR_NEW("failed to create GtkBuilder"));
            } else {
                result = (MgGtkViewResult) OK(view);
            }
        }
    }
    if (!result.is_ok)
        MgGtkView_free(view);

    // Run GTK app in other thread (because run function is blocking)
    ThreadArg* thread_arg = (ThreadArg*)malloc(sizeof(ThreadArg));
    *thread_arg = (ThreadArg) { .data = gdata, .view = view };
    pthread_create(&view->thread, NULL, (void*)app_run_function, thread_arg);
    view->is_thread_running = true;

    return result;
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
    if (view->app && G_IS_APPLICATION(view->app))
        g_application_quit(G_APPLICATION(view->app));

    if (view->is_thread_running)
        pthread_join(view->is_thread_running, NULL);

    if (view->failed_error)
        g_error_free(view->failed_error);

    G_UNREF_SAFE(view->builder);
    G_UNREF_SAFE(view->app);
    if (view->resource)
        g_resources_unregister(view->resource); 

    free(view);
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

    // Allow dropdown
    const GtkTargetEntry target_entries[] = {{ .target = "text/uri-list", 0, 0 }};
    gtk_drag_dest_set(GTK_WIDGET(window), GTK_DEST_DEFAULT_ALL, target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY | GDK_ACTION_MOVE);

    // Show window
    g_signal_connect(window, "destroy", G_CALLBACK(handle_destroy), view);
    gtk_window_set_application(window, view->app);

    // Apply CSS styles
    {
        GtkCssProvider *provider = gtk_css_provider_new();
        gtk_css_provider_load_from_resource(provider, "/org/ussur/mazegtk/style.css");
        GdkScreen *screen = gtk_window_get_screen(window);
        gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_object_unref(provider);
    }

    gtk_widget_show_all(GTK_WIDGET(window));
}