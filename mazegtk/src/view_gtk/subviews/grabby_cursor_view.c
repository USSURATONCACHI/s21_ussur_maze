#include <mazegtk/view_gtk/subviews/grabby_cursor_view.h>
#include <mazegtk/util/common_macros.h>
#include <better_c_std/prettify.h>
#include <gtk/gtk.h>



static gboolean h_enter(GtkWidget* widget, GdkEventCrossing* event, MgGrabbyCursorView* view);
static gboolean h_leave(GtkWidget* widget, GdkEventCrossing* event, MgGrabbyCursorView* view);

MgGrabbyCursorViewResult MgGrabbyCursorView_create(GtkBuilder* ui, const char* widget_name) {
    if (ui == NULL)
        return (MgGrabbyCursorViewResult) ERR(GERROR_NEW("No GtkBuilder provided"));
    if (widget_name == NULL)
        return (MgGrabbyCursorViewResult) ERR(GERROR_NEW("No widget name provided"));

    // Get required UI element
    GtkWidget* widget = GTK_WIDGET(gtk_builder_get_object(ui, widget_name));
    if (widget == NULL) 
        return (MgGrabbyCursorViewResult) ERR(GERROR_NEW("No `%s` widget provided", widget_name));

    gtk_widget_add_events(widget, gtk_widget_get_events(widget) | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);

    return (MgGrabbyCursorViewResult) OK(MgGrabbyCursorView_from_widget(widget));
}

MgGrabbyCursorView* MgGrabbyCursorView_from_widget(GtkWidget* widget) {
    assert_m(widget != NULL);
    
    MgGrabbyCursorView* view = (void*) malloc(sizeof(MgGrabbyCursorView));
    assert_alloc(view);

    *view = (MgGrabbyCursorView) {
        .widget = widget,
    };

    // Connect
    g_signal_connect(view->widget, "enter-notify-event", G_CALLBACK(h_enter), view);
    g_signal_connect(view->widget, "leave-notify-event", G_CALLBACK(h_leave), view);

    return view;
}

void MgGrabbyCursorView_free(MgGrabbyCursorView* view) {
    g_signal_handlers_disconnect_by_data(view->widget, view);
    free(view);
}

// ==
// Handlers
// ==

// Enter
static gboolean h_enter(GtkWidget* widget, GdkEventCrossing* event, MgGrabbyCursorView* view) { 
    unused(event);
    unused(view);

    GdkCursor *hand_cursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_FLEUR);
    gdk_window_set_cursor(gtk_widget_get_window(widget), hand_cursor);
    g_object_unref(G_OBJECT(hand_cursor));
    
    return FALSE; 
}

// Leave
static gboolean h_leave(GtkWidget* widget, GdkEventCrossing* event, MgGrabbyCursorView* view) { 
    unused(event);
    unused(view);
    
    gdk_window_set_cursor(gtk_widget_get_window(widget), NULL);
    
    return FALSE; 
}