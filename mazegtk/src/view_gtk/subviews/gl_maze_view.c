#include <mazegtk/view_gtk/subviews/gl_maze_view.h>
#include <mazegtk/util/common_macros.h>

#include <better_c_std/result.h>
#include <better_c_std/prettify.h>

// Local
typedef STRUCT_RESULT(GlProgram, GError*) GlProgramResult;
typedef STRUCT_RESULT(Shader, GError*) ShaderResult;

static ShaderResult load_shader_from_resource(GResource* resource, const char* resource_path, GLenum shader_type);
static GlProgramResult load_shader_program(GResource* resource, const char* vert_path, const char* frag_path);
static Mesh create_fullscreen_mesh();

static void upload_maze_to_gpu(MgGlMazeView* self);
static void resize_framebuffer(MgGlMazeView* self);
static void generate_fb_mipmaps(MgGlMazeView* self);
static void render_pass_main(MgGlMazeView* self);
static void render_pass_post_proc(MgGlMazeView* self);
// Local

MgGlMazeViewResult MgGlMazeView_create(
    GtkBuilder* ui, GResource* resource, 
    MgMazeController* mazectl, 
    MgCameraController* cameractl,
    MgRenderSettingsStore* settings_store
) {
    if (ui == NULL)             return (MgGlMazeViewResult) ERR(GERROR_NEW("No GtkBuilder provided"));
    if (mazectl == NULL)        return (MgGlMazeViewResult) ERR(GERROR_NEW("No MgMazeController provided"));
    if (cameractl == NULL)      return (MgGlMazeViewResult) ERR(GERROR_NEW("No MgCameraController provided"));
    if (settings_store == NULL) return (MgGlMazeViewResult) ERR(GERROR_NEW("No MgRenderSettingsStore provided"));
    // `resource` can be null, that will just lead to an error return from failing to load shaders.

    GtkGLArea* gl_area = GTK_GL_AREA(gtk_builder_get_object(ui, "gl_area"));
    if (gl_area == NULL)
        return (MgGlMazeViewResult) ERR(GERROR_NEW("No widget `gl_area`"));

    GlProgramResult main_shader = load_shader_program(resource, "/org/ussur/mazegtk/common.vert", "/org/ussur/mazegtk/basic.frag");
    if (!main_shader.is_ok)
        return (MgGlMazeViewResult) ERR(main_shader.error);

    GlProgramResult pp_shader = load_shader_program(resource, "/org/ussur/mazegtk/common.vert", "/org/ussur/mazegtk/post_processing.frag");
    if (!pp_shader.is_ok) {
        gl_program_free(main_shader.ok);
        return (MgGlMazeViewResult) ERR(pp_shader.error);
    }

    MgGlMazeView* view = (void*) malloc(sizeof(MgGlMazeView));
    assert_alloc(view);

    *view = (MgGlMazeView) {
        .mazectl = mazectl,
        .cameractl = cameractl,
        .settings_store = settings_store,
        .gl_area = gl_area,

        .fullscreen_mesh        = create_fullscreen_mesh(), 
        .main_shader            = main_shader.ok,
        .post_processing_shader = pp_shader.ok,

        .maze_ssbo = MazeSsbo_create(),

        .render_buffer = framebuffer_create(10, 10),
        .fb_width  = 10,
        .fb_height = 10,
    };


    gtk_widget_add_events(
        GTK_WIDGET(view->gl_area), 
        gtk_widget_get_events(GTK_WIDGET(view->gl_area)) |
        GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | 
        GDK_SCROLL_MASK | GDK_FOCUS_CHANGE_MASK
    );

    return (MgGlMazeViewResult) OK(view);
}

void MgGlMazeView_free(MgGlMazeView* view) {
    if (view == NULL)
        return;

    if (view->gl_area) g_signal_handlers_disconnect_by_data(view->gl_area, view);

    gl_program_free(view->main_shader);
    gl_program_free(view->post_processing_shader);
    framebuffer_free(view->render_buffer);
    mesh_delete(view->fullscreen_mesh);
    MazeSsbo_free(view->maze_ssbo);

    free(view);
}

void MgGlMazeView_render(MgGlMazeView* view) {
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(view->gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(view->gl_area));

    // Make sure data is up to date
    resize_framebuffer(view);
    if (MgMazeController_was_maze_updated(view->mazectl) || MgMazeController_is_loading(view->mazectl)) {
        upload_maze_to_gpu(view);
        MgMazeController_maze_was_updated(view->mazectl);
    }

    // Render pass 1 : to internal framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, view->render_buffer.framebuffer);
    glViewport(0, 0, view->fb_width, view->fb_height);
    render_pass_main(view);

    // Mipmaps (for MSAA effect)
    generate_fb_mipmaps(view);

    // Render pass 2 : to GtkGLArea
    gtk_gl_area_attach_buffers(view->gl_area);
    glViewport(0, 0, width, height);
    render_pass_post_proc(view);

    // Bugs arise without this line (faulty videodriver? fauly GTK GL?)
    //      I do not fully understand the logic behind this bug, but it
    //      requires the program to set glUseProgram for this shader AFTER rendering.
    glUseProgram(view->main_shader.program);
}


static void resize_framebuffer(MgGlMazeView* self) {
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(self->gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(self->gl_area));

    long double msaa_coef = self->settings_store->msaa_resolution_coef;
    size_t fb_width = (size_t)(width * msaa_coef);
    size_t fb_height = (size_t)(height * msaa_coef);

    if (fb_width != self->fb_width || fb_height != self->fb_height) {
        framebuffer_resize(&self->render_buffer, fb_width, fb_height);
        self->fb_width = fb_width;
        self->fb_height = fb_height;
    }
}
static void generate_fb_mipmaps(MgGlMazeView* self) {
    glBindTexture(GL_TEXTURE_2D, self->render_buffer.color_texture);
    glGenerateMipmap(GL_TEXTURE_2D);
}

static void render_pass_main(MgGlMazeView* self) {
    // Get values for uniforms
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(self->gl_area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(self->gl_area));

    MgVector2 cam_pos = MgCameraController_get_pos(self->cameractl);
    float cell_size = MgCameraController_get_cell_size(self->cameractl);
    
    // Set uniform values
    GLint loc_screen_width = glGetUniformLocation(self->main_shader.program, "u_screen_size");
    GLint loc_cell_size    = glGetUniformLocation(self->main_shader.program, "u_cell_size_pix");
    GLint loc_camera_pos   = glGetUniformLocation(self->main_shader.program, "u_camera_pos");
    glUniform2f(loc_screen_width, (float) width, (float) height);
    glUniform2f(loc_cell_size, cell_size, cell_size);
    glUniform2f(loc_camera_pos, cam_pos.x, cam_pos.y);

    // Bind SSBO into shaders (somewhat like uniforms too)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, self->maze_ssbo.data_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, self->maze_ssbo.size_buffer);

    // And call the render finally
    glUseProgram(self->main_shader.program);
    mesh_bind(self->fullscreen_mesh);
    mesh_draw(self->fullscreen_mesh);
}

static void render_pass_post_proc(MgGlMazeView* self) {
    // Get values for uniforms
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, self->render_buffer.color_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Bind uniforms
    glUseProgram(self->post_processing_shader.program);
    int loc = glGetUniformLocation(self->post_processing_shader.program, "u_read_texture");
    glUniform1i(loc, 0); // 0 as in GL_TEXTURE0 above

    // Call the render
    mesh_bind(self->fullscreen_mesh);
    mesh_draw(self->fullscreen_mesh);
}


static void upload_maze_to_gpu(MgGlMazeView* self) {
    MazeSsbo_upload(
        &self->maze_ssbo, 
        MgMazeController_width(self->mazectl),
        MgMazeController_height(self->mazectl),
        MgMazeController_data_size(self->mazectl),
        MgMazeController_data_buffer(self->mazectl) 
    );
}

// Local
static GlProgramResult load_shader_program(GResource* resource, const char* vert_path, const char* frag_path) {
    debugln("Loading shader program from '%s' + '%s'...", vert_path, frag_path);
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