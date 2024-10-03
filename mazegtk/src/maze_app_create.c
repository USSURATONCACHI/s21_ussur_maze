#include "better_c_std/string/str_t.h"
#include "glib-object.h"
#include "gtk/gtk.h"
#include "opengl_utils/gl_program.h"
#include "opengl_utils/shader.h"
#include <better_c_std/prettify/debug.h>
#include <epoxy/gl_generated.h>
#include <libmaze/maze_struct.h>
#include <string.h>
#include <gio/gio.h>
#include <glib.h>

#include <mazegtk/maze_app.h>
#include <mazegtk/domain.h>
#include <mazegtk/vec_g_error.h>
#include <mazegtk/error_list.h>

#include <better_c_std/prettify.h>
#include <opengl_utils/framebuffer.h>


#define PUTERR(out_error) if (out_error) (*(out_error))


// Alloc app
// Load GResource
// Create GtkApplication
// Create GtkBuilder
// Create read framebuffer
// Create write framebuffer
// Load main shader
// Create fullscreen_mesh
// Load references from ui

static MgMazeApp* alloc_app_uninit(GError** out_error);
static GResource* register_resource(GError** out_error);
static GtkApplication* create_gtk_app(GError** out_error, gpointer callbacks_user_data);
static GtkBuilder* create_gtk_builder(GError** out_error);
static Framebuffer create_framebuffer(int width, int height, GError** out_error);
static GlProgram load_main_shader_program(GResource* resource, GError** out_error);
static Mesh create_fullscreen_mesh(GError** out_error);
static void null_out_ui_references(MgMazeApp* maze_app);

void MgMazeApp_free(MgMazeApp* this) {
    if (this == NULL)
        return;
    
    mg_viewmodel_state_free(&this->state);

    debugln("Unrefing GtkBuilder...");
    g_object_unref(this->private.builder);

    debugln("Unrefing GtkApplication...");
    g_object_unref(this->private.app);

    debugln("Unregistering resource...");
    if (this->private.resource)
        g_resources_unregister(this->private.resource);

    debugln("Freeing maze app...");
    free(this);
}

MgMazeApp* MgMazeApp_create(GError** out_error) {
    debugln("MgMazeApp_create called");
    vec_GError_ptr errors = vec_GError_ptr_create();

    // Allocate
    MgMazeApp* maze_app = alloc_app_uninit(error_list_get_nullptr(&errors));
    memset(maze_app, 0, sizeof(MgMazeApp));
    if (error_list_errors_count(&errors) == 0) {
        maze_app->private.resource = register_resource(error_list_get_nullptr(&errors));
        maze_app->private.app      = create_gtk_app(error_list_get_nullptr(&errors), maze_app);
        maze_app->private.builder  = create_gtk_builder(error_list_get_nullptr(&errors));

        null_out_ui_references(maze_app);
        maze_app->private.read_framebuffer  = (Framebuffer){};
        maze_app->private.write_framebuffer = (Framebuffer){};
        maze_app->private.main_shader       = (GlProgram){};
        maze_app->private.fullscreen_mesh   = (Mesh){};
        maze_app->private.read_framebuffer  = (Framebuffer){};
    }
    int err_count = error_list_errors_count(&errors);
    if (err_count > 0) {
        if (maze_app) {
            MgMazeApp_free(maze_app);
            maze_app = NULL;
        }
        error_list_print_errors(&errors);
        error_list_first_to_g_error(&errors, str_literal("Failed to create MazeApp"), out_error);
    }
    vec_GError_ptr_free(errors);
    return maze_app;
}

static MgMazeApp* alloc_app_uninit(GError** out_error) {
    debugln("Allocating MgMazeApp...");
    MgMazeApp* maze_app = (MgMazeApp*) malloc(sizeof(MgMazeApp));
    if (maze_app == NULL) {
        *out_error = g_error_new(DOMAIN, 0, "failed to allocated memory for MgMazeApp.");
    }
    return maze_app;
}
static GResource* register_resource(GError** out_error) {
    debugln("Loading and registering resource...");

    const char* res_file = getenv(RESOURCES_ENV_VAR);
    debugln("Env variable '%s' is: '%s'", RESOURCES_ENV_VAR, res_file);

    if (res_file == NULL)
        res_file = RESOURCES_DEFAULT_FILE;

    GResource* resource = g_resource_load(res_file, out_error);

    if (resource) {
        g_message("Loaded resource from '%s'", res_file);
        g_resources_register(resource);
    }

    return resource;
}
static GtkApplication* create_gtk_app(GError** out_error, gpointer callbacks_user_data) {
    debugln("Creating GtkApplication...");
    GtkApplication* app = gtk_application_new("org.ussur.mazegtk", G_APPLICATION_DEFAULT_FLAGS);

    if (app == NULL) {
        PUTERR(out_error) = g_error_new_literal(DOMAIN, 0, "failed to create a GTK app.");
    } else {
        g_application_set_resource_base_path(G_APPLICATION (app), "/org/ussur/mazegtk/");
        g_signal_connect(app, "activate", G_CALLBACK(mg_maze_app_handle_activate), callbacks_user_data);
    }

    return app;
}
static GtkBuilder* create_gtk_builder(GError** out_error) {
    debugln("Creating GtkBuilder...");
    GtkBuilder* builder = gtk_builder_new_from_resource("/org/ussur/mazegtk/main.glade");
    if (builder == NULL) {
        PUTERR(out_error) = g_error_new_literal(DOMAIN, 0, "failed to create a builder from resource.");
    }
    return builder;

}

static Framebuffer create_framebuffer(int width, int height, GError** out_error) {
    debugln("Creating Framebuffer...");
    return framebuffer_create(width, height, 1);
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

static void null_out_ui_references(MgMazeApp* maze_app) {
    maze_app->private.ref_main_window = NULL;
    maze_app->private.ref_main_window_stack = NULL;
    maze_app->private.ref_loading_ui = NULL;
    maze_app->private.ref_loading_text = NULL;
    maze_app->private.ref_dropdown_ui = NULL;
    maze_app->private.ref_show_maze_ui = NULL;
}

// Other

void mg_maze_app_handle_activate(GtkWidget* widget, MgMazeApp* maze_app) { MgMazeApp_activate(maze_app); }

static void fill_out_ui_references(MgMazeApp* this, GError** error);

#define GETREF(variable, type, builder, name, out_error)                                 \
    variable = type(gtk_builder_get_object((builder), (name)));                          \
    if (variable == NULL && out_error) {                                                 \
        *(out_error) = g_error_new(DOMAIN, 2, "Did not find widget %s in builder", name);\
    }

void MgMazeApp_activate(MgMazeApp* this) {
    debugln("MgMazeApp_activate called");
    vec_GError_ptr errors = vec_GError_ptr_create();

    GtkWindow* window;
    GETREF(window, GTK_WINDOW, this->private.builder, "main_window", error_list_get_nullptr(&errors));
    GETREF(this->private.ref_main_window,       GTK_WINDOW,  this->private.builder, "main_window",       error_list_get_nullptr(&errors));
    GETREF(this->private.ref_main_window_stack, GTK_STACK,   this->private.builder, "main_window_stack", error_list_get_nullptr(&errors));
    GETREF(this->private.ref_loading_text,      GTK_LABEL,   this->private.builder, "loading_ui_text",   error_list_get_nullptr(&errors));
    GETREF(this->private.ref_dropdown_ui,       GTK_WIDGET,  this->private.builder, "dropdown_ui",       error_list_get_nullptr(&errors));
    GETREF(this->private.ref_show_maze_ui,      GTK_WIDGET,  this->private.builder, "show_maze_ui",      error_list_get_nullptr(&errors));
    GETREF(this->private.ref_gl_area,           GTK_GL_AREA, this->private.builder, "gl_area",           error_list_get_nullptr(&errors));
    this->private.ref_main_window = window;

    int err_count = error_list_errors_count(&errors);
    if (err_count > 0) {
        error_list_print_errors(&errors);
        MgMazeApp_destroy(this);
        return;
    }
    vec_GError_ptr_free(errors);

    // Other
    gtk_builder_connect_signals(this->private.builder, this);
    const GtkTargetEntry target_entries[] = {
        (GtkTargetEntry) { .target = "text/uri-list", 0, 0 },
    };
    gtk_drag_dest_set(GTK_WIDGET(window), GTK_DEST_DEFAULT_ALL, target_entries, G_N_ELEMENTS(target_entries), GDK_ACTION_COPY | GDK_ACTION_MOVE);
    gtk_window_set_application(window, this->private.app);
    gtk_widget_show_all(GTK_WIDGET(window));

    MgViewmodelStateShowMaze new_state = {};
    mg_viewmodel_state_set(&this->state, new_state);
    MgMazeApp_update_shown_state(this);
}


void MgMazeApp_gl_realize(MgMazeApp* self) {
    debugln("MgMazeApp_gl_realize called");
    gtk_gl_area_make_current(self->private.ref_gl_area);

    vec_GError_ptr errors = vec_GError_ptr_create();

    self->private.fb_width          = 0;
    self->private.fb_height         = 0;
    self->private.read_framebuffer  = (Framebuffer){0, 0};
    self->private.write_framebuffer = (Framebuffer){0, 0};
    // self->private.read_framebuffer  = create_framebuffer(self->private.fb_width, self->private.fb_height, error_list_get_nullptr(&errors));
    // self->private.write_framebuffer = create_framebuffer(self->private.fb_width, self->private.fb_height, error_list_get_nullptr(&errors));

        debugln("Gl error: %d", glGetError()); 
    self->private.main_shader       = load_main_shader_program(self->private.resource, error_list_get_nullptr(&errors));
        debugln("Gl error: %d", glGetError()); 
    self->private.fullscreen_mesh   = create_fullscreen_mesh  (error_list_get_nullptr(&errors));
        debugln("Gl error: %d", glGetError()); 

    int err_count = error_list_errors_count(&errors);
    if (err_count > 0) {
        error_list_print_errors(&errors);
        MgMazeApp_destroy(self);
        debugln("MgMazeApp_gl_realize failed.");
        return;
    }
    vec_GError_ptr_free(errors);
    debugln("MgMazeApp_gl_realize done.");
}

void MgMazeApp_gl_unrealize(MgMazeApp* self) {
    debugln("MgMazeApp_gl_unrealize called");
    gtk_gl_area_make_current(self->private.ref_gl_area);
    
    mesh_delete(self->private.fullscreen_mesh);
    gl_program_free(self->private.main_shader);
    framebuffer_free(self->private.write_framebuffer);
    framebuffer_free(self->private.read_framebuffer);
}

G_MODULE_EXPORT void mg_maze_app_handle_gl_realize(void* widget, MgMazeApp* app) {
    MgMazeApp_gl_realize(app);
}

G_MODULE_EXPORT void mg_maze_app_handle_gl_unrealize(void* widget, MgMazeApp* app) {
    MgMazeApp_gl_unrealize(app);
}