#include <mazegtk/view/gtk_view.h>
#include <mazegtk/util/common_macros.h>
#include <mazegtk/controller/controller.h>

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
    uint8_t* maze_data = MgController_get_maze_data_buffer(view->controller);
    size_t maze_width = MgController_get_maze_width(view->controller);
    size_t maze_height = MgController_get_maze_height(view->controller);
    size_t maze_data_size = MgController_get_maze_height(view->controller);

    // Maze itself
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, view->maze_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, maze_data_size, maze_data, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, view->maze_ssbo);

    // Maze size
    GLuint maze_size[2] = { maze_width, maze_height };
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, view->maze_size_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(maze_size), maze_size, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, view->maze_size_ssbo);

    // Screen data
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(gl_area));

    GLint loc_screen_width = glGetUniformLocation(view->main_shader.program, "u_screen_size");
    glUniform2f(loc_screen_width, (float) width, (float) height);
    // -- 

    glViewport(0, 0, width, height);
    glClearColor (0.0, 0, 0, 1);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(view->main_shader.program);
    mesh_bind(view->fullscreen_mesh);
    mesh_draw(view->fullscreen_mesh);
    mesh_unbind();
}