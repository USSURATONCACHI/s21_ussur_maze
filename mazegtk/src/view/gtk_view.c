#include <math.h> 

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
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(gl_area));

    uint8_t* maze_data = MgController_get_maze_data_buffer(view->controller);
    size_t maze_width = MgController_get_maze_width(view->controller);
    size_t maze_height = MgController_get_maze_height(view->controller);
    size_t maze_data_size = MgController_get_maze_height(view->controller);

    MgCameraController* camera = MgController_get_camera(view->controller);
    // MgCameraController_update_anim(camera);
    MgVector2 cam_pos = MgCameraController_pos(camera);
    MgVector2 cam_vel = MgCameraController_vel(camera);
    float     cam_zoom = MgCameraController_zoom(camera);

    float cell_size = powf(1.1f, cam_zoom);

    debugln("Cam position: %f %f Zoom : %f", cam_pos.x, cam_pos.y, cam_zoom);

    GLint loc_maze_pos = glGetUniformLocation(view->main_shader.program, "u_maze_pos_pix");
    GLint loc_cell_size = glGetUniformLocation(view->main_shader.program, "u_cell_size_pix");
    glUniform2f(loc_maze_pos, ((float) width) / 2.0 - cam_pos.x, ((float) width) / 2.0 - cam_pos.y);
    glUniform2f(loc_cell_size, cell_size, cell_size);

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

G_MODULE_EXPORT void handle_gl_area_drag_begin(GtkGLArea* gl_area) {
    debugln(__PRETTY_FUNCTION__);
}
G_MODULE_EXPORT void handle_gl_area_drag_end(GtkGLArea* gl_area) {
    debugln(__PRETTY_FUNCTION__);
}


G_MODULE_EXPORT gboolean handle_gl_area_enter(GtkGLArea* gl_area, GdkEventCrossing* event, MgGtkView* view) { 
    debugln(__PRETTY_FUNCTION__); 
    GdkCursor *hand_cursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_FLEUR);
    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(gl_area)), hand_cursor);
    g_object_unref(G_OBJECT(hand_cursor));
    return FALSE; 
}
G_MODULE_EXPORT gboolean handle_gl_area_leave(GtkGLArea* gl_area, GdkEventCrossing* event, MgGtkView* view) { 
    debugln(__PRETTY_FUNCTION__); 
    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(gl_area)), NULL);
    return FALSE; 
}

static void get_cursor(GtkGLArea* gl_area, gint* x, gint* y) {
    GdkWindow* gw = gtk_widget_get_window(GTK_WIDGET(gl_area));
    GdkDisplay *display = gdk_display_get_default();
    GdkSeat* seat = gdk_display_get_default_seat(display);
    GdkDevice* pointer = gdk_seat_get_pointer(seat);
    GdkModifierType mask;
    gdk_window_get_device_position(gw, pointer, x, y, &mask);
}

G_MODULE_EXPORT gboolean handle_gl_area_button_press(GtkGLArea* gl_area, GdkEventButton* event, MgGtkView* view) { 
    debugln(__PRETTY_FUNCTION__);
    MgCameraController* camera = MgController_get_camera(view->controller);
    MgCameraController_update_anim(camera);
    MgCameraController_on_drag_start(camera);
    view->is_dragging = true;

    gint x, y;
    get_cursor(gl_area, &x, &y);
    view->prev_x = x;
    view->prev_y = y;

    return FALSE; 
}

G_MODULE_EXPORT gboolean handle_gl_area_button_release(GtkGLArea* gl_area, GdkEventButton* event, MgGtkView* view) { 
    debugln(__PRETTY_FUNCTION__); 
    MgCameraController* camera = MgController_get_camera(view->controller);
    // MgCameraController_update_anim(camera);
    MgCameraController_on_drag_end(camera);
    view->is_dragging = false;
    return FALSE;
}
G_MODULE_EXPORT void handle_gl_area_motion(GtkGLArea* gl_area, GdkEventMotion* event, MgGtkView* view) {
    if (view->is_dragging) {
        gint x, y;
        get_cursor(gl_area, &x, &y);

        int dx = x - view->prev_x;
        int dy = y - view->prev_y;

        view->prev_x = x;
        view->prev_y = y;

        MgCameraController* camera = MgController_get_camera(view->controller);
        MgCameraController_on_drag(camera, (MgVector2) { -dx, -dy });
    }
}

#define SCROLL_SENSITIVITY 0.1

G_MODULE_EXPORT gboolean handle_gl_area_scroll(GtkGLArea* gl_area, GdkEventScroll* event, MgGtkView* view) { 
    // debugln("%lf %lf", (double)event->delta_x, (double)event->delta_y); 
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

    view->is_dragging = false;
    return TRUE;
}