#include <mazegtk/view/gtk_view.h>
#include <better_c_std/prettify.h>

static void view_free_sig_unrealize(MgGtkView* view);
static void view_free_actually(MgGtkView* view);


void MgGtkView_free_sync(MgGtkView* view) {
    if (view == NULL)
        return;

    if (view->app)
        g_application_quit(G_APPLICATION(view->app));

    view_free_actually(view);
}

static void view_free_actually(MgGtkView* view) {
    debugln("view_free_actually called");
    if (view == NULL)
        return;

    if (G_IS_APPLICATION(view->app)) {
        g_application_quit(G_APPLICATION(view->app));
    }
    
    debugln("Unrefing GtkBuilder...");
    g_object_unref(view->builder);

    debugln("Unrefing GtkApplication...");
    g_object_unref(view->app);

    debugln("Unregistering resource...");
    if (view->resource)
        g_resources_unregister(view->resource);

    debugln("Freeing maze app...");
    free(view);
    
    debugln("view_free_actually done");
}

void MgGtkView_handle_destroy(void* dont_care, MgGtkView* view) {
    debugln("MgGtkView_handle_destroy called");
    if (G_IS_APPLICATION(view->app)) {
        g_application_quit(G_APPLICATION(view->app));
    }
    view->is_stopped = true;
}