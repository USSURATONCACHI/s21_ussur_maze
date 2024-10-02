#include <mazegtk/viewmodel.h>
#include <mazegtk/maze_app.h>
#include <mazegtk/domain.h>

#include <better_c_std/prettify.h>

#include <stdbool.h>

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include <epoxy/gl.h>

void MgMazeApp_free(MgMazeApp* this) {
    if (this == NULL)
        return;
    
    mg_viewmodel_state_free(&this->state);
    g_object_unref(this->private.builder);
    g_object_unref(this->private.app);
    g_resources_unregister(this->private.resource);
    free(this);
}

void MgMazeApp_activate(MgMazeApp* this) {
    gtk_builder_connect_signals(this->private.builder, this);

    // Window
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(this->private.builder, "main_window"));
    if (window == NULL) {
        g_error("No main_window object found in UI layout");
        MgMazeApp_destroy(this);
        return;
    }
    const GtkTargetEntry target_entries[] = {
        (GtkTargetEntry) { .target = "text/uri-list", 0, 0 },
    };
    gtk_drag_dest_set(GTK_WIDGET(window), GTK_DEST_DEFAULT_ALL, target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY | GDK_ACTION_MOVE);

    gtk_window_set_application(window, this->private.app);
    gtk_widget_show_all(GTK_WIDGET(window));

    // Other elements
    GtkStack* main_window_stack = GTK_STACK(gtk_builder_get_object(this->private.builder, "main_window_stack"));
    GtkWidget* main_ui = GTK_WIDGET(gtk_builder_get_object(this->private.builder, "main_ui"));

    this->private.ref_main_window = window;
    this->private.ref_main_window_stack = GTK_STACK (gtk_builder_get_object(this->private.builder, "main_window_stack"));
    this->private.ref_show_maze_ui      = GTK_WIDGET(gtk_builder_get_object(this->private.builder, "show_maze_ui"));
    this->private.ref_loading_text      = GTK_LABEL (gtk_builder_get_object(this->private.builder, "loading_ui_text"));
    this->private.ref_dropdown_ui       = GTK_WIDGET(gtk_builder_get_object(this->private.builder, "dropdown_ui"));
    this->private.ref_show_maze_ui      = GTK_WIDGET(gtk_builder_get_object(this->private.builder, "show_maze_ui"));

    MgViewmodelStateShowMaze new_state = {};
    mg_viewmodel_state_set(&this->state, new_state);
    MgMazeApp_update_shown_state(this);
}


void MgMazeApp_update_shown_state(MgMazeApp* this) {
    switch (this->state.type) {
        case MAZEGTK_VIEWMODEL_STATE_LOADING:
            gtk_stack_set_visible_child(this->private.ref_main_window_stack, this->private.ref_loading_ui);
            gtk_label_set_text(this->private.ref_loading_text, this->state.loading.text_shown);
            break;

        case MAZEGTK_VIEWMODEL_STATE_DRAG:
            gtk_stack_set_visible_child(this->private.ref_main_window_stack, this->private.ref_dropdown_ui);
            break;
            
        case MAZEGTK_VIEWMODEL_STATE_SHOW_MAZE:
            gtk_stack_set_visible_child(this->private.ref_main_window_stack, this->private.ref_show_maze_ui);
            break;
    }
}

void MgMazeApp_destroy(MgMazeApp* this) {
    g_info("Destroying the Maze App");
    g_application_quit(G_APPLICATION(this->private.app));
}

void MgMazeApp_run(MgMazeApp* this, int argc, char** argv) {
    g_application_run(G_APPLICATION(this->private.app), argc, argv);
}

void MgMazeApp_drag_moved(MgMazeApp* this) {
    MgViewmodelStateDrag new_state = {};
    mg_viewmodel_state_set(&this->state, new_state);
    MgMazeApp_update_shown_state(this);
}
void MgMazeApp_drag_ended(MgMazeApp* this) {
    MgViewmodelStateShowMaze new_state = {};
    mg_viewmodel_state_set(&this->state, new_state);
    MgMazeApp_update_shown_state(this);
}

bool MgMazeApp_render_gl(MgMazeApp* this, GtkGLArea* widget, GdkGLContext* context) {
    glClearColor (0, 0, 1.0, 0);
    glClear (GL_COLOR_BUFFER_BIT);
    return true;
}

G_MODULE_EXPORT void mg_maze_app_handle_drag_move(
    GtkWidget* widget, 
    GdkDragContext* context,
    gint x,
    gint y,
    guint time,
    MgMazeApp* maze_app
) { 
    MgMazeApp_drag_moved(maze_app);
}
G_MODULE_EXPORT void mg_maze_app_handle_drag_leave(
    GtkWidget* widget, 
    GdkDragContext* context,
    guint time,
    MgMazeApp* maze_app
) { 
    MgMazeApp_drag_ended(maze_app);
}

G_MODULE_EXPORT bool mg_maze_app_handle_render_gl(
    GtkGLArea* widget, 
    GdkGLContext* context,
    MgMazeApp* maze_app
) {
    return MgMazeApp_render_gl(maze_app, widget, context);
}

void mg_maze_app_handle_activate(GtkWidget* widget, MgMazeApp* maze_app) { MgMazeApp_activate(maze_app); }
void mg_maze_app_handle_destroy(GtkWidget *widget, MgMazeApp* maze_app)  { MgMazeApp_destroy(maze_app); }