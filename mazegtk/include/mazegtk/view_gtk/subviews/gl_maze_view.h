#ifndef MAZEGTK_VIEW_GTK_SUBVIEWS_GL_MAZE_VIEW_H_
#define MAZEGTK_VIEW_GTK_SUBVIEWS_GL_MAZE_VIEW_H_

#include <mazegtk/controller/maze.h>
#include <better_c_std/result.h>
#include <gtk/gtk.h>

#include <opengl_utils/mesh.h>
#include <opengl_utils/gl_program.h>
#include <opengl_utils/framebuffer.h>
#include <mazegtk/view_gtk/maze_ssbo.h>

typedef struct {
    MgMazeController* controller;

    GtkGLArea* gl_area;

    Mesh        fullscreen_mesh;
    GlProgram   main_shader;
    GlProgram   post_processing_shader;
    MazeSsbo    maze_ssbo;
    
    Framebuffer render_buffer;
    size_t fb_width, fb_height;
} MgGlMazeView;

typedef STRUCT_RESULT(MgGlMazeView*, GError*) MgGlMazeViewResult;

MgGlMazeViewResult MgGlMazeView_create(GtkBuilder* ui, GResource* resource, MgMazeController* controller);
void MgGlMazeView_free(MgGlMazeView* view);

void MgGlMazeView_render(MgGlMazeView* view);

#endif // MAZEGTK_VIEW_GTK_SUBVIEWS_GL_MAZE_VIEW_H_