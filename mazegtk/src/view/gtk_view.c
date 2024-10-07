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

    // Render pass 1 - to internal framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, view->inner.render_buffer.framebuffer);
    glViewport(0, 0, view->inner.fb_width, view->inner.fb_height);
    render_pass_main(gl_area, view);

    // Mipmaps (for MSAA effect)
    generate_fb_mipmaps(view);

    // Render pass 2 - to gl area
    gtk_gl_area_attach_buffers(gl_area);
    glViewport(0, 0, width, height);
    render_pass_post_proc(gl_area, view);

    // Bugs arise without this line because (faulty videodriver)
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

    // Update X value
    UPDATE_GDOUBLE(view->builder, "cam_pos_x", view->inner.last_shown.cam_x,    cam_pos.x);
    UPDATE_GDOUBLE(view->builder, "cam_pos_y", view->inner.last_shown.cam_y,    cam_pos.y);
    UPDATE_GDOUBLE(view->builder, "cam_zoom",  view->inner.last_shown.zoom, cam_zoom );

    UPDATE_GDOUBLE(view->builder, "drag_sensitivity", view->inner.last_shown.drag_sensitivity, view->inner.drag_sensitivity);
    UPDATE_GDOUBLE(view->builder, "zoom_speed", view->inner.last_shown.zoom_speed, view->inner.zoom_speed);
}


// Camera UI user input
G_MODULE_EXPORT void handle_cam_pos_x_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.cam_x)
        return;

    MgCameraController* camera = MgController_get_camera(view->controller);
    MgVector2 cam_pos = MgCameraController_pos(camera);
    cam_pos.x = val;
    MgCameraController_set_pos(camera, cam_pos);
}

G_MODULE_EXPORT void handle_cam_pos_y_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.cam_y)
        return;

    MgCameraController* camera = MgController_get_camera(view->controller);
    MgVector2 cam_pos = MgCameraController_pos(camera);
    cam_pos.y = val;
    MgCameraController_set_pos(camera, cam_pos);
}

G_MODULE_EXPORT void handle_cam_zoom_value_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.zoom)
        return;

    MgCameraController* camera = MgController_get_camera(view->controller);
    MgCameraController_set_zoom(camera, val);
}

G_MODULE_EXPORT void handle_reset_camera(GtkSpinButton* widget, MgGtkView* view) {
    MgMazeController* maze = MgController_get_maze(view->controller, 0);
    size_t width = MgMazeController_width(maze);
    size_t height = MgMazeController_height(maze);

    MgCameraController* camera = MgController_get_camera(view->controller);
    MgCameraController_stop_movement(camera);
    MgCameraController_set_zoom(camera, 20.0);
    MgCameraController_set_pos(camera, (MgVector2) { width / 2.0f, height / 2.0f });
}

// Camera controls UI input
G_MODULE_EXPORT void handle_drag_sensitivity_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.drag_sensitivity)
        return;
    view->inner.drag_sensitivity = val;
}

G_MODULE_EXPORT void handle_zoom_speed_changed(GtkSpinButton* widget, MgGtkView* view) {
    gdouble val = gtk_spin_button_get_value(widget);
    if (val == view->inner.last_shown.cam_y)
        return;
    view->inner.zoom_speed = val;
}

G_MODULE_EXPORT void cam_controls_reset_to_default(GtkButton* widget, MgGtkView* view) {
    view->inner.zoom_speed = 1.25;
    view->inner.drag_sensitivity = 1.0;
}





// Grabby cursor
G_MODULE_EXPORT gboolean handle_gl_area_enter(GtkGLArea* gl_area, GdkEventCrossing* event, MgGtkView* view) { 
    GdkCursor *hand_cursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_FLEUR);
    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(gl_area)), hand_cursor);
    g_object_unref(G_OBJECT(hand_cursor));
    return FALSE; 
}
G_MODULE_EXPORT gboolean handle_gl_area_leave(GtkGLArea* gl_area, GdkEventCrossing* event, MgGtkView* view) { 
    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(gl_area)), NULL);
    return FALSE; 
}

// Mouse controls
static void get_cursor(GtkGLArea* gl_area, gint* x, gint* y) {
    GdkWindow* gw = gtk_widget_get_window(GTK_WIDGET(gl_area));
    GdkDisplay *display = gdk_display_get_default();
    GdkSeat* seat = gdk_display_get_default_seat(display);
    GdkDevice* pointer = gdk_seat_get_pointer(seat);
    GdkModifierType mask;
    gdk_window_get_device_position(gw, pointer, x, y, &mask);
}

G_MODULE_EXPORT gboolean handle_gl_area_button_press(GtkGLArea* gl_area, GdkEventButton* event, MgGtkView* view) { 
    MgCameraController* camera = MgController_get_camera(view->controller);
    MgCameraController_update_anim(camera);
    MgCameraController_on_drag_start(camera);
    view->inner.is_dragging = true;

    gint x, y;
    get_cursor(gl_area, &x, &y);
    view->inner.prev_x = x;
    view->inner.prev_y = y;
    gtk_widget_grab_focus(GTK_WIDGET(gl_area));

    return FALSE; 
}

G_MODULE_EXPORT gboolean handle_gl_area_button_release(GtkGLArea* gl_area, GdkEventButton* event, MgGtkView* view) { 
    MgCameraController* camera = MgController_get_camera(view->controller);
    MgCameraController_on_drag_end(camera);
    view->inner.is_dragging = false;
    return FALSE;
}
G_MODULE_EXPORT void handle_gl_area_motion(GtkGLArea* gl_area, GdkEventMotion* event, MgGtkView* view) {
    if (view->inner.is_dragging) {
        gint x, y;
        get_cursor(gl_area, &x, &y);

        int dx = x - view->inner.prev_x;
        int dy = y - view->inner.prev_y;
        view->inner.prev_x = x;
        view->inner.prev_y = y;

        double drag_sens = view->inner.drag_sensitivity;

        MgCameraController* camera = MgController_get_camera(view->controller);
        float cell_size = powf(view->inner.zoom_speed, MgCameraController_zoom(camera));
        MgCameraController_on_drag(camera, (MgVector2) { -dx * drag_sens / cell_size, -dy * drag_sens / cell_size });
    }
}


#define SCROLL_SENSITIVITY 0.1

G_MODULE_EXPORT gboolean handle_gl_area_scroll(GtkGLArea* gl_area, GdkEventScroll* event, MgGtkView* view) { 
    MgCameraController* camera = MgController_get_camera(view->controller);

    MgCameraController_update_anim(camera);
    switch (event->direction) {
        case GDK_SCROLL_UP:    MgCameraController_on_zoom(camera, 10.0 * SCROLL_SENSITIVITY); break;
        case GDK_SCROLL_RIGHT: MgCameraController_on_zoom(camera, 10.0 * SCROLL_SENSITIVITY); break;
        case GDK_SCROLL_DOWN:  MgCameraController_on_zoom(camera, -10.0 * SCROLL_SENSITIVITY); break;
        case GDK_SCROLL_LEFT:  MgCameraController_on_zoom(camera, -10.0 * SCROLL_SENSITIVITY); break;
        case GDK_SCROLL_SMOOTH:
            MgCameraController_on_zoom(camera, (event->delta_x + event->delta_y) * SCROLL_SENSITIVITY);
            break;
    }

    view->inner.is_dragging = false;
    return TRUE;
}

// Dropdown controls
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