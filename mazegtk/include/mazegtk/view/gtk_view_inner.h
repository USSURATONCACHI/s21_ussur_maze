#ifndef MAZEGTK_VIEW_GTK_VIEW_INNER_H_
#define MAZEGTK_VIEW_GTK_VIEW_INNER_H_

#include <mazegtk/controller/controller.h>
#include <mazegtk/util/result.h>
#include <mazegtk/view/maze_ssbo.h>
#include <opengl_utils/gl_program.h>
#include <opengl_utils/mesh.h>
#include <opengl_utils/framebuffer.h>

#include <glib.h>
#include <gio/gio.h>

typedef struct {
    MgController* controller;

    // For rendering
    Mesh        fullscreen_mesh;
    GlProgram   main_shader;
    GlProgram   post_processing_shader;
    Framebuffer render_buffer;
    MazeSsbo    maze_ssbo;

    double msaa_coef;
    size_t fb_width, fb_height;

    // For camera & camera controls
    bool is_dragging;
    int prev_x, prev_y; // can be uninitialized
    double drag_sensitivity;
    double zoom_speed;

    // For maze generation
    size_t gen_maze_w, gen_maze_h;

    // For camera UI (can be uninit)
    struct {
        gdouble cam_x, cam_y;
        gdouble zoom;
        gdouble zoom_speed;
        gdouble drag_sensitivity;

        gdouble msaa_coef;

        size_t gen_maze_w, gen_maze_h;
    } last_shown;
} MgGtkViewInner;

typedef STRUCT_RESULT(MgGtkViewInner, GError*) MgGtkViewInnerResult;

MgGtkViewInnerResult MgGtkViewInner_new(MgController* controller, GResource* resource);
void MgGtkViewInner_free(MgGtkViewInner view_inner);

void MgGtkViewInner_upload_maze_to_gpu(MgGtkViewInner* view_inner);

#endif // MAZEGTK_VIEW_GTK_VIEW_INNER_H_