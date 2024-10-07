#include <mazegtk/view/gtk_view.h>
#include <mazegtk/view/gtk_view_inner.h>
#include <math.h>

#include <better_c_std/prettify.h>

static void update_camera_ui(GtkGLArea* gl_area, MgGtkView* view);
static void resize_framebuffer(GtkGLArea* gl_area, MgGtkView* view);

static void generate_fb_mipmaps(MgGtkView* view);
static void render_pass_main(GtkGLArea* gl_area, MgGtkView* view);
static void render_pass_post_proc(GtkGLArea* gl_area, MgGtkView* view);

G_MODULE_EXPORT void mg_maze_app_handle_render_gl(GtkGLArea* gl_area, GdkGLContext* context, MgGtkView* view) {
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(gl_area));

    update_camera_ui(gl_area, view);
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
    MgVector2 cam_pos = MgCameraController_pos(camera);
    float cam_zoom = MgCameraController_zoom(camera);
    float cell_size = powf(view->inner.zoom_speed, cam_zoom);
    
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
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(gl_area));

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


#define UPDATE_GDOUBLE(builder, name, last_shown_var, new_value) \
    {                                                                                             \
        GtkSpinButton* updating_btn = GTK_SPIN_BUTTON(gtk_builder_get_object((builder), (name))); \
        if (updating_btn && !gtk_widget_is_focus(GTK_WIDGET(updating_btn))) {                     \
            (last_shown_var) = (new_value);                                                       \
            gtk_spin_button_set_value(updating_btn, (last_shown_var));                            \
        }                                                                                         \
    }                                                                                             \
    //;

static void update_camera_ui(GtkGLArea* gl_area, MgGtkView* view) {
    MgCameraController* camera = MgController_get_camera(view->controller);
    MgVector2 cam_pos = MgCameraController_pos(camera);
    float cam_zoom = MgCameraController_zoom(camera);

    UPDATE_GDOUBLE(view->builder, "cam_pos_x", view->inner.last_shown.cam_x, cam_pos.x);
    UPDATE_GDOUBLE(view->builder, "cam_pos_y", view->inner.last_shown.cam_y, cam_pos.y);
    UPDATE_GDOUBLE(view->builder, "cam_zoom",  view->inner.last_shown.zoom, cam_zoom);

    UPDATE_GDOUBLE(view->builder, "drag_sensitivity", view->inner.last_shown.drag_sensitivity, view->inner.drag_sensitivity);
    UPDATE_GDOUBLE(view->builder, "zoom_speed", view->inner.last_shown.zoom_speed, view->inner.zoom_speed);

    UPDATE_GDOUBLE(view->builder, "msaa_coef", view->inner.last_shown.msaa_coef, view->inner.msaa_coef);

    UPDATE_GDOUBLE(view->builder, "gen_maze_width", view->inner.last_shown.gen_maze_w, view->inner.gen_maze_w);
    UPDATE_GDOUBLE(view->builder, "gen_maze_height", view->inner.last_shown.gen_maze_h, view->inner.gen_maze_h);

}

G_MODULE_EXPORT void handle_msaa_coef_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.msaa_coef)
        return;
    view->inner.msaa_coef = val;
}
G_MODULE_EXPORT void handle_gen_maze_width_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.gen_maze_w)
        return;
    view->inner.gen_maze_w = val;
}
G_MODULE_EXPORT void handle_gen_maze_height_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.gen_maze_h)
        return;
    view->inner.gen_maze_h = val;
}

G_MODULE_EXPORT void handle_generate_eller(GtkWidget* widget, MgGtkView* view) {
    MgController_create_maze_eller(view->controller, view->inner.gen_maze_w, view->inner.gen_maze_h);
    MgGtkViewInner_upload_maze_to_gpu(&view->inner);
}
G_MODULE_EXPORT void handle_generate_empty(GtkWidget* widget, MgGtkView* view) {
    MgController_create_maze_empty(view->controller, view->inner.gen_maze_w, view->inner.gen_maze_h);
    MgGtkViewInner_upload_maze_to_gpu(&view->inner);
}
G_MODULE_EXPORT void handle_generate_cropped(GtkWidget* widget, MgGtkView* view) {
    
}