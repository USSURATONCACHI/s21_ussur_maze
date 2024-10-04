#include <mazegtk/view/gtk_view.h>
#include <better_c_std/prettify.h>
#include <mazegtk/util/getref.h>
#include <mazegtk/util/error_list.h>

#include <opengl_utils/gl_program.h>

static GlProgram load_main_shader_program(GResource* resource, GError** out_error);
static Mesh create_fullscreen_mesh(GError** out_error);


G_MODULE_EXPORT void mg_maze_app_handle_gl_unrealize(GtkGLArea* widget, MgGtkView* view) {
    debugln("mg_maze_app_handle_gl_unrealize called");
    gtk_gl_area_make_current(widget);

    mesh_delete(view->fullscreen_mesh);
    gl_program_free(view->main_shader);
    debugln("mg_maze_app_handle_gl_unrealize done");
}

G_MODULE_EXPORT void mg_maze_app_handle_gl_realize(GtkGLArea* widget, MgGtkView* view) {
    debugln("mg_maze_app_handle_gl_realize called");
    vec_GError_ptr errors = vec_GError_ptr_create();
    gtk_gl_area_make_current(widget);

    view->main_shader = load_main_shader_program(view->resource, error_list_get_nullptr(&errors));
    view->fullscreen_mesh = create_fullscreen_mesh(error_list_get_nullptr(&errors));
    glGenBuffers(1, &view->maze_ssbo);

    if (error_list_errors_count(&errors) > 0) {
        debugln("Failed to gl_realize.");
        error_list_print_errors(&errors);
        vec_GError_ptr_free(errors);

        gl_program_free(view->main_shader);
        mesh_delete(view->fullscreen_mesh);
        glDeleteBuffers(1, &view->maze_ssbo);

        MgGtkView_handle_destroy(NULL, view); // Stop the app
        return;
    } else {
        vec_GError_ptr_free(errors);
        debugln("mg_maze_app_handle_gl_realize done.");
    }
}


static GlProgram load_main_shader_program(GResource* resource, GError** out_error) {
    debugln("Loading main shader program...");

    if (resource == NULL) {
        if (out_error)
            *out_error = g_error_new(DOMAIN, 3, "No resource loaded to load shader program");
        return (GlProgram) {};
    }

    Shader vert;
    {
        GBytes* bytes = g_resource_lookup_data(resource, "/org/ussur/mazegtk/common.vert", G_RESOURCE_LOOKUP_FLAGS_NONE, out_error);
        debugln("Looked up: %p", bytes);
        if (!bytes) {
            if (out_error)
                *out_error = g_error_new(DOMAIN, 3, "Failed to load file data for vertex shader");
            return (GlProgram) {};
        }
        gsize size;
        const gchar* filetext = (const gchar*)g_bytes_get_data(bytes, &size);

        vert = shader_from_source(GL_VERTEX_SHADER, filetext);
        g_bytes_unref(bytes);
    }

    Shader frag;
    {
        GBytes* bytes = g_resource_lookup_data(resource, "/org/ussur/mazegtk/basic.frag", G_RESOURCE_LOOKUP_FLAGS_NONE, out_error);
        debugln("Looked up: %p", bytes);
        if (!bytes) {
            if (out_error)
                *out_error = g_error_new(DOMAIN, 3, "Failed to load file data for fragment shader");
            shader_free(vert);
            return (GlProgram) {};
        }
        gsize size;
        const gchar* filetext = (const gchar*)g_bytes_get_data(bytes, &size);

        frag = shader_from_source(GL_FRAGMENT_SHADER, filetext);
        g_bytes_unref(bytes);
    }

    GlProgram program = gl_program_from_2_shaders(&vert, &frag);
    shader_free(vert);
    shader_free(frag);
    return program;
}

static Mesh create_fullscreen_mesh(GError** out_error) {
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