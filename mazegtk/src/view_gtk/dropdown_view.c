#include <mazegtk/view_gtk/dropdown_view.h>
#include <mazegtk/util/common_macros.h>
#include <better_c_std/prettify.h>


static gboolean h_drag_move(GtkWidget* widget, GdkDragContext* context, gint x, gint y, guint time, MgDropdownView* view);
static gboolean h_drag_leave(GtkWidget* widget, GdkDragContext* context, guint time, MgDropdownView* view);

MgDropdownViewResult MgDropdownView_create(GtkBuilder* ui, MgController* controller) {
    assert_m(ui != NULL);
    assert_m(controller != NULL);

    // Get required UI elements
    GtkWindow* window      = GTK_WINDOW(gtk_builder_get_object(ui, "main_window"));
    GtkWidget* dropdown_ui = GTK_WIDGET(gtk_builder_get_object(ui, "dropdown_ui"));
    GtkStack*  main_stack  = GTK_STACK(gtk_builder_get_object(ui, "main_stack"));
    
    if (window == NULL)      return (MgDropdownViewResult) ERR(GERROR_NEW("No `main_window` provided"));
    if (dropdown_ui == NULL) return (MgDropdownViewResult) ERR(GERROR_NEW("No `dropdown_ui` provided"));
    if (main_stack == NULL)  return (MgDropdownViewResult) ERR(GERROR_NEW("No `main_stack` provided"));

    // Allocate
    MgDropdownView* view = (void*) malloc(sizeof(MgDropdownView));
    assert_alloc(view);
    *view = (MgDropdownView) {
        .controller = controller,
        
        .window      = window,
        .dropdown_ui = dropdown_ui,
        .main_stack  = main_stack,

        .last_visible_child = NULL,
    };

    // Allow dropdown
    const GtkTargetEntry target_entries[] = {
        { .target = "text/uri-list", 0, 0 }
    };
    gtk_drag_dest_set(GTK_WIDGET(view->window), GTK_DEST_DEFAULT_ALL, target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY | GDK_ACTION_MOVE);

    // Connect
    g_signal_connect(view->window, "drag-motion", G_CALLBACK(h_drag_move),  view);
    g_signal_connect(view->window, "drag-leave",  G_CALLBACK(h_drag_leave), view);

    return (MgDropdownViewResult) OK(view);
}

void MgDropdownView_free(MgDropdownView* view) {
    g_signal_handlers_disconnect_by_data(view->window, view);
    free(view);
}


// ==
// Handlers
// ==

// Move
static gboolean h_drag_move(GtkWidget* widget, GdkDragContext* context, gint x, gint y, guint time, MgDropdownView* view) {
    unused(widget);
    unused(context);
    unused(time);
    unused(x);
    unused(y);

    GtkWidget* visible = gtk_stack_get_visible_child(view->main_stack);

    if (visible != view->dropdown_ui) {
        view->last_visible_child = visible;
        gtk_stack_set_visible_child(view->main_stack, view->dropdown_ui);
    }

    return TRUE;
}

// Leave
static gboolean h_drag_leave(GtkWidget* widget, GdkDragContext* context, guint time, MgDropdownView* view) {
    unused(widget);
    unused(context);
    unused(time);

    if (view->last_visible_child) {
        gtk_stack_set_visible_child(view->main_stack, view->last_visible_child);
        view->last_visible_child = NULL;
    }

    return TRUE;
}