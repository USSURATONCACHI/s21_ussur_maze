#include <mazegtk/view/gtk_view.h>

#include <mazegtk/util/error_list.h>
#include <mazegtk/util/common_macros.h>

#include <better_c_std/prettify.h>
#include <better_c_std/string.h>

#include <pthread.h>
#include <time.h>
#include <unistd.h>

static MgGtkView* alloc_view_uninit(GError** out_error);
static GResource* register_resource(GError** out_error);
static GtkApplication* create_gtk_app(GError** out_error, gpointer callbacks_user_data);
static GtkBuilder* create_gtk_builder(GError** out_error);

static void* run_gtk_app(void* view);
static void handle_activate(void* widget, MgGtkView* view);

typedef struct {
    MgGtkView* view;
    int argc;
    char** argv;
} ThreadArgument;

MgGtkView* MgGtkView_create_sync(MgController* controller, int argc, char** argv, GError** out_error) {
    debugln("MgGtkView_create_sync called");
    vec_GError_ptr errors = vec_GError_ptr_create();

    // Allocate
    MgGtkView* maze_app = alloc_view_uninit(error_list_get_nullptr(&errors));

    if (error_list_errors_count(&errors) == 0) {
        maze_app->controller = controller;
        maze_app->is_stopped = false;
        maze_app->resource = register_resource(error_list_get_nullptr(&errors));
        maze_app->app      = create_gtk_app(error_list_get_nullptr(&errors), maze_app);
        maze_app->builder  = create_gtk_builder(error_list_get_nullptr(&errors));
    }
    if (error_list_errors_count(&errors) > 0) {
        error_list_print_errors(&errors);
        error_list_first_to_g_error(&errors, str_literal("Failed to create MazeApp"), out_error);
        vec_GError_ptr_free(errors);
        MgGtkView_free_sync(maze_app);
        return NULL;
    }

    // Continue into startup & realize state.
    bool is_activate_done = false;
    maze_app->waits.is_activate_done = &is_activate_done;
    maze_app->waits.create_errors_list = &errors;
    
    ThreadArgument* arg = (ThreadArgument*)malloc(sizeof(ThreadArgument));
    assert_alloc(arg);
    *arg = (ThreadArgument){
        .view = maze_app,
        .argc = argc,
        .argv = argv,
    };
    pthread_t thread;
    int ret = pthread_create(&thread, NULL, run_gtk_app, arg);
    
    debugln("Waiting for GTK to activate and realize my application...");
    while (!is_activate_done) {
        usleep(1000);
    }

    maze_app->waits.is_activate_done = NULL;
    maze_app->waits.create_errors_list = NULL;

    debugln("View is done creating, checking...");
    if (error_list_errors_count(&errors) > 0) {
        debugln("Failed to create MgGtkView. Reasons: ");
        error_list_print_errors(&errors);
        error_list_first_to_g_error(&errors, str_literal("Failed to create MazeApp"), out_error);
        vec_GError_ptr_free(errors);
        MgGtkView_free_sync(maze_app);
        return NULL;
    }
    debugln("No errors encountered");

    return maze_app;
}

static void* run_gtk_app(void* arg_in) {
    ThreadArgument* arg_heap = arg_in;
    ThreadArgument arg = *arg_heap;
    free(arg_heap);

    g_application_run(G_APPLICATION(arg.view->app), arg.argc, arg.argv);

    return NULL;
}

static MgGtkView* alloc_view_uninit(GError** out_error) {
    debugln("Allocating MgGtkView...");
    MgGtkView* maze_app = (MgGtkView*) calloc(1, sizeof(MgGtkView));
    if (maze_app == NULL) {
        *out_error = g_error_new(DOMAIN, 0, "failed to allocated memory for MgGtkView.");
    }
    return maze_app;
}
static GResource* register_resource(GError** out_error) {
    debugln("Loading and registering resource...");

    const char* res_file = getenv(RESOURCES_ENV_VAR);
    debugln("Env variable '%s' is: '%s'", RESOURCES_ENV_VAR, res_file);

    if (res_file == NULL)
        res_file = RESOURCES_DEFAULT_FILE;

    GResource* resource = g_resource_load(res_file, out_error);

    if (resource) {
        g_message("Loaded resource from '%s'", res_file);
        g_resources_register(resource);
    }

    return resource;
}
static GtkApplication* create_gtk_app(GError** out_error, gpointer callbacks_user_data) {
    debugln("Creating GtkApplication...");
    GtkApplication* app = gtk_application_new("org.ussur.mazegtk", G_APPLICATION_DEFAULT_FLAGS);

    if (app == NULL) {
        PUTERR(out_error) = g_error_new_literal(DOMAIN, 0, "failed to create a GTK app.");
    } else {
        g_application_set_resource_base_path(G_APPLICATION (app), "/org/ussur/mazegtk/");
        g_signal_connect(app, "activate", G_CALLBACK(handle_activate), callbacks_user_data);
    }

    return app;
}
static GtkBuilder* create_gtk_builder(GError** out_error) {
    debugln("Creating GtkBuilder...");
    GtkBuilder* builder = gtk_builder_new_from_resource("/org/ussur/mazegtk/main.glade");
    if (builder == NULL) {
        PUTERR(out_error) = g_error_new_literal(DOMAIN, 0, "failed to create a builder from resource.");
    }
    return builder;

}

#define GETREF(variable, type, builder, name, out_error)                                 \
    variable = type(gtk_builder_get_object((builder), (name)));                          \
    if (variable == NULL && out_error) {                                                 \
        *(out_error) = g_error_new(DOMAIN, 2, "Did not find widget %s in builder", name);\
    }

static void handle_activate(void* widget, MgGtkView* view) { 
    debugln("handle_activate called");
    vec_GError_ptr* errors = view->waits.create_errors_list;

    GtkWindow* window;
    GETREF(window, GTK_WINDOW, view->builder, "main_window", error_list_get_nullptr(errors));

    int err_count = error_list_errors_count(errors);
    if (err_count > 0) {
        debugln("handle_activate failed");
        *(view->waits.is_activate_done) = true;
        return;
    }

    // Load and apply the CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(provider, "/org/ussur/mazegtk/style.css");
    GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(window));
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);

    // Other
    gtk_builder_connect_signals(view->builder, view);
    const GtkTargetEntry target_entries[] = {
        (GtkTargetEntry) { .target = "text/uri-list", 0, 0 },
    };
    gtk_drag_dest_set(GTK_WIDGET(window), GTK_DEST_DEFAULT_ALL, target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY | GDK_ACTION_MOVE);
    gtk_window_set_application(window, view->app);
    gtk_widget_show_all(GTK_WIDGET(window));
    g_signal_connect(window, "destroy", G_CALLBACK(MgGtkView_handle_destroy), view);

    *(view->waits.is_activate_done) = true;
    debugln("handle_activate done");
}