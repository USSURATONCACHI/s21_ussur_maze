#include <mazegtk/view/gtk_view.h>
#include <mazegtk/util/getref.h>

bool MgGtkView_is_fine(const MgGtkView* view) {
    return !view->is_stopped;
}

gboolean MgGtkView_startup_prepare(MgGtkView* view) {
    
    GtkStack* stack = GTK_STACK(gtk_builder_get_object(view->builder, "main_stack"));
    GtkWidget* show_maze_ui = GTK_WIDGET(gtk_builder_get_object(view->builder, "show_maze_ui"));

    gtk_stack_set_visible_child(stack, show_maze_ui);

    return true;
}