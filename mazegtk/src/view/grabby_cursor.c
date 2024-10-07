#include <mazegtk/view/gtk_view.h>
#include <gtk/gtk.h>

G_MODULE_EXPORT gboolean handle_gl_area_enter(GtkGLArea* gl_area, GdkEventCrossing* event, MgGtkView* view) { 
    unused(event);
    unused(view);
    GdkCursor *hand_cursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_FLEUR);
    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(gl_area)), hand_cursor);
    g_object_unref(G_OBJECT(hand_cursor));
    return FALSE; 
}
G_MODULE_EXPORT gboolean handle_gl_area_leave(GtkGLArea* gl_area, GdkEventCrossing* event, MgGtkView* view) { 
    unused(event);
    unused(view);
    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(gl_area)), NULL);
    return FALSE; 
}