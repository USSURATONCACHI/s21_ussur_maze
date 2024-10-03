#include "better_c_std/prettify/debug.h"
#include "gdk/gdk.h"
#include "opengl_utils/framebuffer.h"
#include "opengl_utils/mesh.h"
#include <epoxy/gl_generated.h>
#include <mazegtk/viewmodel.h>
#include <mazegtk/maze_app.h>
#include <mazegtk/domain.h>

#include <better_c_std/prettify.h>

#include <stdbool.h>

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include <epoxy/gl.h>

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
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(widget));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(widget));
    
    debug_push();
    glViewport(0, 0, width, height);
    glClearColor (0.5, 0, 0, 1);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL_INVALID_ENUM;
    glUseProgram(this->private.main_shader.program);
    mesh_bind(this->private.fullscreen_mesh);
    mesh_draw(this->private.fullscreen_mesh);
    mesh_unbind();

    debug_pop();
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

void mg_maze_app_handle_destroy(GtkWidget *widget, MgMazeApp* maze_app)  { MgMazeApp_destroy(maze_app); }