#include <gtk/gtk.h>
#include <mazegtk/view_gtk/gtk_view.h>
#include <math.h>

static void resize_framebuffer(GtkGLArea* gl_area, MgGtkView* view);

static void generate_fb_mipmaps(MgGtkView* view);
static void render_pass_main(GtkGLArea* gl_area, MgGtkView* view);
static void render_pass_post_proc(GtkGLArea* gl_area, MgGtkView* view);



G_MODULE_EXPORT void mg_maze_app_handle_render_gl(GtkGLArea* gl_area, GdkGLContext* context, MgGtkView* view) {
    mg_maze_app_handle_update_ui(gl_area, context, view);
    unused(context);
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(gl_area));

    resize_framebuffer(gl_area, view);

    // Render pass 1 : to internal framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, view->inner.render_buffer.framebuffer);
    glViewport(0, 0, view->inner.fb_width, view->inner.fb_height);
    render_pass_main(gl_area, view);

    // Mipmaps (for MSAA effect)
    generate_fb_mipmaps(view);

    // Render pass 2 : to GtkGLArea
    gtk_gl_area_attach_buffers(gl_area);
    glViewport(0, 0, width, height);
    render_pass_post_proc(gl_area, view);

    // Bugs arise without this line (because of faulty videodriver)
    //      I do not fully understand the logic behind this bug, but it
    //      requires the program to set glUseProgram for my shader AFTER rendering.
    glUseProgram(view->inner.main_shader.program);
}

static void resize_framebuffer(GtkGLArea* gl_area, MgGtkView* view)    {
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(gl_area));
    

    size_t fb_width = (size_t)(width * view->inner.msaa_coef);
    size_t fb_height = (size_t)(height * view->inner.msaa_coef);

    if (fb_width != view->inner.fb_width || fb_height != view->inner.fb_height) {
        framebuffer_resize(&view->inner.render_buffer, fb_width, fb_height, 1);
        view->inner.fb_width = fb_width;
        view->inner.fb_height = fb_height;
    }
}

static void generate_fb_mipmaps(MgGtkView* view) {
    glBindTexture(GL_TEXTURE_2D, view->inner.render_buffer.color_texture);
    glGenerateMipmap(GL_TEXTURE_2D);
}

static void render_pass_main(GtkGLArea* gl_area, MgGtkView* view) {
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(gl_area));

    MgCameraController* camera = MgController_get_camera(view->controller);
    MgVector2 cam_pos = MgCameraController_get_pos(camera);
    float cell_size = MgCameraController_get_cell_size(camera);
    
    GLint loc_screen_width = glGetUniformLocation(view->inner.main_shader.program, "u_screen_size");
    GLint loc_cell_size    = glGetUniformLocation(view->inner.main_shader.program, "u_cell_size_pix");
    GLint loc_camera_pos   = glGetUniformLocation(view->inner.main_shader.program, "u_camera_pos");
    glUniform2f(loc_screen_width, (float) width, (float) height);
    glUniform2f(loc_cell_size, cell_size, cell_size);
    glUniform2f(loc_camera_pos, cam_pos.x, cam_pos.y);

    const MazeSsbo* ssbo = &view->inner.maze_ssbo;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo->data_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo->size_buffer);

    glUseProgram(view->inner.main_shader.program);
    mesh_bind(view->inner.fullscreen_mesh);
    mesh_draw(view->inner.fullscreen_mesh);
}

static void render_pass_post_proc(GtkGLArea* gl_area, MgGtkView* view) {
    unused(gl_area);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, view->inner.render_buffer.color_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUseProgram(view->inner.post_processing_shader.program);
    int loc = glGetUniformLocation(view->inner.post_processing_shader.program, "u_read_texture");
    glUniform1i(loc, 0);
    mesh_bind(view->inner.fullscreen_mesh);
    mesh_draw(view->inner.fullscreen_mesh);
}

