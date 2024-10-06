#include <mazegtk/view/gtk_view.h>
#include <mazegtk/view/gtk_view_inner.h>
#include <math.h>

#include <better_c_std/prettify.h>

#define ZOOM_EXP 1.2f

G_MODULE_EXPORT void mg_maze_app_handle_render_gl(GtkGLArea* gl_area, GdkGLContext* context, MgGtkView* view) {
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(gl_area));

    MgCameraController* camera = MgController_get_camera(view->controller);
    MgVector2 cam_pos = MgCameraController_pos(camera);
    float cam_zoom = MgCameraController_zoom(camera);
    float cell_size = powf(ZOOM_EXP, cam_zoom);
    // debugln("Cam position: %f %f Zoom : %f", cam_pos.x, cam_pos.y, cam_zoom);
    // glBindFramebuffer(GL_FRAMEBUFFER, view->inner.render_buffer.framebuffer);

    // Mipmap
    const MazeSsbo* ssbo = &view->inner.maze_ssbo;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo->data_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo->size_buffer);

    // Uniforms
    GLint loc_screen_width = glGetUniformLocation(view->inner.main_shader.program, "u_screen_size");
    GLint loc_cell_size    = glGetUniformLocation(view->inner.main_shader.program, "u_cell_size_pix");
    GLint loc_camera_pos   = glGetUniformLocation(view->inner.main_shader.program, "u_camera_pos");
    glUniform2f(loc_screen_width, (float) width, (float) height);
    glUniform2f(loc_cell_size, cell_size, cell_size);
    glUniform2f(loc_camera_pos, cam_pos.x, cam_pos.y);

    // === Render
    glClearColor (0.0, 0, 0, 1);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Resize custom framebuffer 
    size_t fb_width = width * 4;
    size_t fb_height = height * 4;

    if (fb_width != view->inner.fb_width || fb_height != view->inner.fb_height) {
        framebuffer_resize(&view->inner.render_buffer, fb_width, fb_height, 1);
        view->inner.fb_width = fb_width;
        view->inner.fb_height = fb_height;
    }

    mesh_bind(view->inner.fullscreen_mesh);
        // Render 1
        // Draw to framebuffer 
        glBindFramebuffer(GL_FRAMEBUFFER, view->inner.render_buffer.framebuffer);
        glViewport(0, 0, fb_width, fb_height);
        glUseProgram(view->inner.main_shader.program);
        mesh_draw(view->inner.fullscreen_mesh);

        // Generate mipmaps for the framebuffer's color texture
        glBindTexture(GL_TEXTURE_2D, view->inner.render_buffer.color_texture);
        glGenerateMipmap(GL_TEXTURE_2D);
    
        // Render 2
        // Draw main
        gtk_gl_area_attach_buffers(gl_area);
        glViewport(0, 0, width, height);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, view->inner.render_buffer.color_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



        glUseProgram(view->inner.post_processing_shader.program);
        int loc = glGetUniformLocation(view->inner.post_processing_shader.program, "u_read_texture");
        glUniform1i(loc, 0);
        mesh_draw(view->inner.fullscreen_mesh);
    
        // Bugs arise without this line:
        glUseProgram(view->inner.main_shader.program);
    mesh_unbind();
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

    return FALSE; 
}

G_MODULE_EXPORT gboolean handle_gl_area_button_release(GtkGLArea* gl_area, GdkEventButton* event, MgGtkView* view) { 
    MgCameraController* camera = MgController_get_camera(view->controller);
    // MgCameraController_update_anim(camera);
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

        MgCameraController* camera = MgController_get_camera(view->controller);
        float cell_size = powf(ZOOM_EXP, MgCameraController_zoom(camera));
        MgCameraController_on_drag(camera, (MgVector2) { -dx / cell_size, -dy / cell_size });
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
