#include <mazegtk/view/gtk_view.h>
#include <gtk/gtk.h>

G_MODULE_EXPORT gboolean mg_maze_app_handle_drag_move(
    GtkWidget* widget, 
    GdkDragContext* context,
    gint x,
    gint y,
    guint time,
    MgGtkView* view
) {
    GtkStack* stack = GTK_STACK(gtk_builder_get_object(view->builder, "main_stack"));
    if (stack == NULL)
        return FALSE;

    GtkWidget* ui = GTK_WIDGET(gtk_builder_get_object(view->builder, "dropdown_ui"));

    gtk_stack_set_visible_child(stack, ui);
    return TRUE;
}

G_MODULE_EXPORT gboolean mg_maze_app_handle_drag_leave(
    GtkWidget* widget, 
    GdkDragContext* context,
    guint time,
    MgGtkView* view
) {
    GtkStack* stack = GTK_STACK(gtk_builder_get_object(view->builder, "main_stack"));
    if (stack == NULL)
        return FALSE;

    GtkWidget* ui = GTK_WIDGET(gtk_builder_get_object(view->builder, "show_maze_ui"));

    gtk_stack_set_visible_child(stack, ui);
    return TRUE;
}