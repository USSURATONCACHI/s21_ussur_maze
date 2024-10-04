#include <mazegtk/view/gtk_view.h>
#include <mazegtk/util/common_macros.h>
#include <mazegtk/controller/controller.h>

#include <libmaze/maze.h>
#include <opengl_utils/mesh.h>

#include <better_c_std/prettify.h>

bool MgGtkView_is_fine(const MgGtkView* view) {
    return !view->is_stopped;
}

gboolean MgGtkView_startup_prepare(MgGtkView* view) {
    debugln("Sigma sigma");
    GtkStack* stack = GTK_STACK(gtk_builder_get_object(view->builder, "main_stack"));
    GtkWidget* show_maze_ui = GTK_WIDGET(gtk_builder_get_object(view->builder, "show_maze_ui"));

    gtk_stack_set_visible_child(stack, show_maze_ui);

    return false;
}

G_MODULE_EXPORT void mg_maze_app_handle_render_gl(GtkGLArea* gl_area, GdkGLContext* context, MgGtkView* view) {
    // -- 
    MzMaze* maze = MgController_get_maze(view->controller);
    uint8_t* maze_data = maze->raw_data;
    size_t maze_data_size = mz_maze_get_buffer_size(maze);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, view->maze_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, maze_data_size, maze_data, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, view->maze_ssbo);
    // -- 
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(gl_area));

    glViewport(0, 0, width, height);
    glClearColor (0.0, 0, 0, 1);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(view->main_shader.program);
    mesh_bind(view->fullscreen_mesh);
    mesh_draw(view->fullscreen_mesh);
    mesh_unbind();
}