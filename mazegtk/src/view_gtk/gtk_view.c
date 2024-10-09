#include <mazegtk/view_gtk/gtk_view.h>
#include <mazegtk/view_gtk/gtk_view_inner.h>
#include <math.h>
#include <mazegtk/util/common_macros.h>

#include <better_c_std/prettify.h>

void MgGtkView_show_error_screen(MgGtkView* view, GError* error) {
    GtkStack* main_stack = GTK_STACK(gtk_builder_get_object(view->builder, "main_stack"));
    if (main_stack == NULL)
        MgGtkView_fail_with_error(view, GERROR_NEW("No `main_stack` widget :("));

    GtkWidget* error_ui = GTK_WIDGET(gtk_builder_get_object(view->builder, "error_ui"));
    if (error_ui == NULL)
        MgGtkView_fail_with_error(view, GERROR_NEW("No `error_ui` widget :("));

    GtkWidget* show_maze_ui = GTK_WIDGET(gtk_builder_get_object(view->builder, "show_maze_ui"));
    assert_m(show_maze_ui != NULL); // This would be absurd if at this point app hasnt crashed cuz of this


    GtkLabel* error_label = GTK_LABEL(gtk_builder_get_object(view->builder, "error_text_label"));
    if (error_ui == NULL)
        MgGtkView_fail_with_error(view, GERROR_NEW("No `error_text_label` widget :("));
    gtk_label_set_text(error_label, error->message);
    g_error_free(error);

    gtk_stack_set_visible_child(main_stack, error_ui);
}

G_MODULE_EXPORT void handle_error_continue(GtkWidget* widget, MgGtkView* view) {
    unused(widget);
    GtkStack* main_stack = GTK_STACK(gtk_builder_get_object(view->builder, "main_stack"));
    assert_m(main_stack != NULL);

    GtkWidget* show_maze_ui = GTK_WIDGET(gtk_builder_get_object(view->builder, "show_maze_ui"));
    assert_m(show_maze_ui != NULL);

    gtk_stack_set_visible_child(main_stack, show_maze_ui);
}


// static void view_set_to_loading_ui(MgGtkView* view) {
//     GtkStack* main_stack = GTK_STACK(gtk_builder_get_object(view->builder, "main_stack"));
//     if (main_stack == NULL)
//         MgGtkView_fail_with_error(view, GERROR_NEW("No `main_stack` widget :("));

//     GtkWidget* loading_ui = GTK_WIDGET(gtk_builder_get_object(view->builder, "loading_ui"));
//     if (loading_ui == NULL)
//         MgGtkView_fail_with_error(view, GERROR_NEW("No `loading_ui` widget :("));

//     GtkWidget* show_maze_ui = GTK_WIDGET(gtk_builder_get_object(view->builder, "show_maze_ui"));
//     assert_m(show_maze_ui != NULL); // This would be absurd if at this point app hasnt crashed cuz of this

//     gtk_stack_set_visible_child(main_stack, loading_ui);
// }

// static void view_react_to_create_result(MgGtkView* view, MgCreateError err) {
//     GtkStack* main_stack = GTK_STACK(gtk_builder_get_object(view->builder, "main_stack"));
//     if (main_stack == NULL)
//         MgGtkView_fail_with_error(view, GERROR_NEW("No `main_stack` widget :("));

//     GtkWidget* loading_ui = GTK_WIDGET(gtk_builder_get_object(view->builder, "loading_ui"));
//     if (loading_ui == NULL)
//         MgGtkView_fail_with_error(view, GERROR_NEW("No `loading_ui` widget :("));

//     GtkWidget* show_maze_ui = GTK_WIDGET(gtk_builder_get_object(view->builder, "show_maze_ui"));
//     assert_m(show_maze_ui != NULL); // This would be absurd if at this point app hasnt crashed cuz of this

//     switch (err) {
//         case MG_SUCCESS:
//             MgGtkViewInner_upload_maze_to_gpu(&view->inner);
//             gtk_stack_set_visible_child(main_stack, show_maze_ui);
//             break;

//         case MG_NOT_ENOUGH_MEMORY:
//             MgGtkView_show_error_screen(view, GERROR_NEW("Not enough RAM :/"));
//             break;
        
//         default:
//             MgGtkView_show_error_screen(view, GERROR_NEW("Something went horribly wrong,\nbut we do not know what"));
//             break;
//     }
// }

G_MODULE_EXPORT gboolean dont_scroll_unless_focused(GtkWidget *widget) {
    if (gtk_widget_is_focus(widget)) {
        return FALSE;
    } else {
        g_signal_stop_emission_by_name(widget, "scroll-event");
        return FALSE;
    }
}

G_MODULE_EXPORT gboolean handle_grab_focus(GtkWidget* widget) { 
    debugln(__PRETTY_FUNCTION__);
    gtk_widget_grab_focus(GTK_WIDGET(widget));
    return FALSE; 
}
