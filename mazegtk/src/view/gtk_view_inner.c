#include <mazegtk/view/gtk_view_inner.h>
#include <mazegtk/util/common_macros.h>
#include <mazegtk/util/result.h>

#include <better_c_std/prettify.h>

// Local
typedef STRUCT_RESULT(GlProgram, GError*) GlProgramResult;
typedef STRUCT_RESULT(Shader, GError*) ShaderResult;

static ShaderResult load_shader_from_resource(GResource* resource, const char* resource_path, GLenum shader_type);
static GlProgramResult load_main_shader_program(GResource* resource, const char* vert_path, const char* frag_path);
static Mesh create_fullscreen_mesh();
// Local


MgGtkViewInnerResult MgGtkViewInner_new(MgController* controller, GResource* resource) {
    GlProgramResult main_shader = load_main_shader_program(resource, "/org/ussur/mazegtk/common.vert", "/org/ussur/mazegtk/basic.frag");
    if (!main_shader.is_ok)
        return (MgGtkViewInnerResult) ERR(main_shader.error);

    GlProgramResult pp_shader = load_main_shader_program(resource, "/org/ussur/mazegtk/common.vert", "/org/ussur/mazegtk/post_processing.frag");
    if (!pp_shader.is_ok) {
        gl_program_free(main_shader.ok);
        return (MgGtkViewInnerResult) ERR(pp_shader.error);
    }

    Mesh fullscreen_mesh = create_fullscreen_mesh();
    Framebuffer framebuffer = framebuffer_create(1000, 1000, 1);

    // Upload mipmaps to GPU
    MazeSsbo maze_ssbo = MazeSsbo_create();
    MgMazeController* maze = MgController_get_maze(controller, 0);
    MazeSsbo_upload(
        &maze_ssbo, 
        MgMazeController_width(maze), 
        MgMazeController_height(maze), 
        MgMazeController_data_size(maze), 
        MgMazeController_data_buffer(maze)
    );

    // Return sucess
    MgGtkViewInner inner = (MgGtkViewInner){
        .controller             = controller,
        .is_dragging            = false,
        .fullscreen_mesh        = fullscreen_mesh,
        .main_shader            = main_shader.ok,
        .post_processing_shader = pp_shader.ok,
        .maze_ssbo              = maze_ssbo,

        .render_buffer = framebuffer,
        .fb_width      = 1000,
        .fb_height     = 1000,
    };
    return (MgGtkViewInnerResult) OK(inner);
}

void MgGtkViewInner_free(MgGtkViewInner view_inner) {
    MazeSsbo_free(view_inner.maze_ssbo);
    framebuffer_free(view_inner.render_buffer);
    mesh_delete(view_inner.fullscreen_mesh);
    gl_program_free(view_inner.post_processing_shader);
    gl_program_free(view_inner.main_shader);
}


// Local
static GlProgramResult load_main_shader_program(GResource* resource, const char* vert_path, const char* frag_path) {
    debugln("Loading main shader program...");
    if (resource == NULL)
        return (GlProgramResult) ERR(GERROR_NEW("No resource loaded to load shader program"));

    ShaderResult vert = load_shader_from_resource(resource, vert_path, GL_VERTEX_SHADER);
    if (!vert.is_ok)
        return (GlProgramResult) ERR(vert.error);

    ShaderResult frag = load_shader_from_resource(resource, frag_path, GL_FRAGMENT_SHADER);
    if (!frag.is_ok) {
        shader_free(vert.ok);
        return (GlProgramResult) ERR(vert.error);
    }


    GlProgram result = gl_program_from_2_shaders(&vert.ok, &frag.ok);
    shader_free(vert.ok);
    shader_free(frag.ok);

    return (GlProgramResult) OK(result);
}

static ShaderResult load_shader_from_resource(GResource* resource, const char* resource_path, GLenum shader_type) {
    GBytes* bytes = g_resource_lookup_data(resource, resource_path, G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
    if (!bytes) {
        return (ShaderResult) ERR(GERROR_NEW("failed to load resource '%s'", resource_path));
    }

    gsize size;
    const gchar* filetext = (const gchar*)g_bytes_get_data(bytes, &size);

    Shader shader = shader_from_source(shader_type, filetext);
    g_bytes_unref(bytes);

    return (ShaderResult) OK(shader);
}

static Mesh create_fullscreen_mesh() {
    Mesh mesh = mesh_create();

    MeshAttrib attribs[] = {
        {2, sizeof(float), GL_FLOAT},
        {2, sizeof(float), GL_FLOAT},
    };
    mesh_bind_consecutive_attribs(mesh, 0, attribs,
                                    sizeof(attribs) / sizeof(attribs[0]));

    float vertices[] = {
        -1.0, -1.0, 0.0, 0.0, 
        1.0,  -1.0, 1.0, 0.0,
        1.0,  1.0,  1.0, 1.0, 
        -1.0, 1.0,  0.0, 1.0,
    };
    mesh_set_vertex_data(&mesh, vertices, sizeof(vertices), GL_STATIC_DRAW);

    int indices[] = {
        0, 1, 2, 0, 3, 2,
    };
    mesh_set_indices_int_tuples(
        &mesh, indices, sizeof(indices) / sizeof(indices[0]), GL_STATIC_DRAW);

    return mesh;
}