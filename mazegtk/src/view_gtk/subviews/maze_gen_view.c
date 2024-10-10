#include <mazegtk/view_gtk/subviews/maze_gen_view.h>
#include <mazegtk/view_gtk/subviews/ui_update_macros.h>
#include <better_c_std/prettify.h>

static void h_gen_w_changed(GtkSpinButton* widget, MgMazeGenView* view);
static void h_gen_h_changed(GtkSpinButton* widget, MgMazeGenView* view);

static void h_gen_random(GtkButton* widget, MgMazeGenView* view);
static void h_gen_eller(GtkButton* widget, MgMazeGenView* view);
static void h_gen_empty(GtkButton* widget, MgMazeGenView* view);
static void h_gen_crop(GtkButton* widget, MgMazeGenView* view);

MgMazeGenView* MgMazeGenView_create(GtkBuilder* ui, MgMazeGenController* controller) {
    if (ui == NULL || controller == NULL)
        return NULL;

    MgMazeGenView* view = (void*) malloc(sizeof(MgMazeGenView));
    assert_alloc(view);

    *view = (MgMazeGenView) {
        .controller = controller,

        .gen_w_btn = GTK_SPIN_BUTTON(gtk_builder_get_object(ui, "gen_maze_width")),
        .gen_h_btn = GTK_SPIN_BUTTON(gtk_builder_get_object(ui, "gen_maze_height")),

        .gen_random_btn = GTK_BUTTON(gtk_builder_get_object(ui, "gen_random")),
        .gen_eller_btn  = GTK_BUTTON(gtk_builder_get_object(ui, "gen_eller")),
        .gen_empty_btn  = GTK_BUTTON(gtk_builder_get_object(ui, "gen_empty")),
        .gen_crop_btn   = GTK_BUTTON(gtk_builder_get_object(ui, "gen_crop")),

        .last_shown = {}, // can be uninitialized, does not matter 
    };

    if (view->gen_w_btn) g_signal_connect(view->gen_w_btn, "value-changed", G_CALLBACK(h_gen_w_changed), view);
    if (view->gen_h_btn) g_signal_connect(view->gen_h_btn, "value-changed", G_CALLBACK(h_gen_h_changed), view);

    if (view->gen_random_btn) g_signal_connect(view->gen_random_btn, "clicked", G_CALLBACK(h_gen_random), view);
    if (view->gen_eller_btn)  g_signal_connect(view->gen_eller_btn, "clicked", G_CALLBACK(h_gen_eller), view);
    if (view->gen_empty_btn)  g_signal_connect(view->gen_empty_btn, "clicked", G_CALLBACK(h_gen_empty), view);
    if (view->gen_crop_btn)   g_signal_connect(view->gen_crop_btn, "clicked", G_CALLBACK(h_gen_crop), view);

    return view;
}

void MgMazeGenView_free(MgMazeGenView* view) {
    debugln(__PRETTY_FUNCTION__);

    if (view->gen_w_btn)     g_signal_handlers_disconnect_by_data(view->gen_w_btn,     view);
    if (view->gen_h_btn)     g_signal_handlers_disconnect_by_data(view->gen_h_btn,     view);
    if (view->gen_random_btn) g_signal_handlers_disconnect_by_data(view->gen_eller_btn, view);
    if (view->gen_eller_btn) g_signal_handlers_disconnect_by_data(view->gen_eller_btn, view);
    if (view->gen_empty_btn) g_signal_handlers_disconnect_by_data(view->gen_empty_btn, view);
    if (view->gen_crop_btn)  g_signal_handlers_disconnect_by_data(view->gen_crop_btn,  view);

    free(view);
    
    debugln("%s done", __PRETTY_FUNCTION__);
}

void MgMazeGenView_update_ui(MgMazeGenView* view) {
    size_t w = MgMazeGenController_get_gen_width(view->controller);
    size_t h = MgMazeGenController_get_gen_height(view->controller);

    UPDATE_SPINBTN_GDOUBLE(view->gen_w_btn, view->last_shown.gen_w, w);
    UPDATE_SPINBTN_GDOUBLE(view->gen_h_btn, view->last_shown.gen_h, h);

    bool is_loading = MgMazeGenController_is_loading(view->controller);
    gtk_widget_set_sensitive(GTK_WIDGET(view->gen_eller_btn),  !is_loading);
    gtk_widget_set_sensitive(GTK_WIDGET(view->gen_crop_btn),   !is_loading);
    gtk_widget_set_sensitive(GTK_WIDGET(view->gen_empty_btn),  !is_loading);
    gtk_widget_set_sensitive(GTK_WIDGET(view->gen_random_btn), !is_loading);
}

// ==
// Handlers
// ==

// Width changed
static void h_gen_w_changed(GtkSpinButton* widget, MgMazeGenView* view) {
    CHECK_GDOUBLE_CHANGED(val, widget, view->last_shown.gen_w);
    MgMazeGenController_set_gen_width(view->controller, val);
}

// Height changed
static void h_gen_h_changed(GtkSpinButton* widget, MgMazeGenView* view) {
    CHECK_GDOUBLE_CHANGED(val, widget, view->last_shown.gen_h);
    MgMazeGenController_set_gen_height(view->controller, val);
}


// Ask if really needed
typedef void (*GenerationFunction)(MgMazeGenController*);

typedef struct {
    GenerationFunction fn;
    MgMazeGenController* controller;
} GenerationUserData;

static void on_response(GtkDialog *dialog, gint response_id, GenerationUserData* user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        user_data->fn(user_data->controller);
    } else if (response_id == GTK_RESPONSE_CANCEL) {
        debugln("Cancel pressed");
    }
    free(user_data);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void show_confirmation_dialog(MgMazeGenView* view, GenerationFunction fn) {
    size_t ram_mb = MgMazeGenController_how_much_ram_needed_mb(view->controller);

    GtkWidget* dialog = gtk_message_dialog_new(
            NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_NONE,
            "This will require %zu MB RAM + %zu MB VRAM. Also may take significant time to allocate/generate. Are you sure?",
            ram_mb, ram_mb
        );
    
    gtk_dialog_add_buttons(
        GTK_DIALOG(dialog),
            "Cancel", GTK_RESPONSE_CANCEL,
            "Continue", GTK_RESPONSE_ACCEPT,
            NULL
        );

    GenerationUserData* user_data = (void*)malloc(sizeof(GenerationUserData));
    *user_data = (GenerationUserData){
        .fn = fn,
        .controller = view->controller,
    };
    g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(on_response), user_data);
    gtk_widget_show(dialog);
}


// Generate random maze
static void h_gen_random(GtkButton* widget, MgMazeGenView* view) {
    unused(widget);
    size_t ram_mb = MgMazeGenController_how_much_ram_needed_mb(view->controller);

    if (ram_mb > 256)
        show_confirmation_dialog(view, MgMazeGenController_gen_random);
    else
        MgMazeGenController_gen_random(view->controller);
}

// Generate maze with Eller's algorithm
static void h_gen_eller(GtkButton* widget, MgMazeGenView* view) {
    unused(widget);
    size_t ram_mb = MgMazeGenController_how_much_ram_needed_mb(view->controller);

    if (ram_mb > 256)
        show_confirmation_dialog(view, MgMazeGenController_gen_eller);
    else
        MgMazeGenController_gen_eller(view->controller);
}

// Generate empty maze
static void h_gen_empty(GtkButton* widget, MgMazeGenView* view) {
    unused(widget);
    size_t ram_mb = MgMazeGenController_how_much_ram_needed_mb(view->controller);

    if (ram_mb > 256)
        show_confirmation_dialog(view, MgMazeGenController_gen_empty);
    else
        MgMazeGenController_gen_empty(view->controller);
}

// Rezise the maze
static void h_gen_crop(GtkButton* widget, MgMazeGenView* view) {
    unused(widget);
    size_t ram_mb = MgMazeGenController_how_much_ram_needed_mb(view->controller);

    if (ram_mb > 256)
        show_confirmation_dialog(view, MgMazeGenController_gen_crop);
    else
        MgMazeGenController_gen_crop(view->controller);
}