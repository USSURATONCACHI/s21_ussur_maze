#include <mazegtk/view_gtk/subviews/maze_gen_view.h>
#include <mazegtk/view_gtk/subviews/ui_update_macros.h>
#include <better_c_std/prettify.h>

static void h_gen_w_changed(GtkSpinButton* widget, MgMazeGenView* view);
static void h_gen_h_changed(GtkSpinButton* widget, MgMazeGenView* view);

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

        .gen_eller_btn = GTK_BUTTON(gtk_builder_get_object(ui, "gen_eller")),
        .gen_empty_btn = GTK_BUTTON(gtk_builder_get_object(ui, "gen_empty")),
        .gen_crop_btn  = GTK_BUTTON(gtk_builder_get_object(ui, "gen_crop")),

        .last_shown = {}, // can be uninitialized, does not matter 
    };

    if (view->gen_w_btn) g_signal_connect(view->gen_w_btn, "value-changed", G_CALLBACK(h_gen_w_changed), view);
    if (view->gen_h_btn) g_signal_connect(view->gen_h_btn, "value-changed", G_CALLBACK(h_gen_h_changed), view);

    if (view->gen_eller_btn) g_signal_connect(view->gen_eller_btn, "clicked", G_CALLBACK(h_gen_eller), view);
    if (view->gen_empty_btn) g_signal_connect(view->gen_empty_btn, "clicked", G_CALLBACK(h_gen_empty), view);
    if (view->gen_crop_btn)  g_signal_connect(view->gen_crop_btn, "clicked", G_CALLBACK(h_gen_crop), view);

    return view;
}

void MgMazeGenView_free(MgMazeGenView* view) {
    debugln(__PRETTY_FUNCTION__);

    if (view->gen_w_btn)     g_signal_handlers_disconnect_by_data(view->gen_w_btn,     view);
    if (view->gen_h_btn)     g_signal_handlers_disconnect_by_data(view->gen_h_btn,     view);
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

// Generate maze with Eller's algorithm
static void h_gen_eller(GtkButton* widget, MgMazeGenView* view) {
    unused(widget);
    MgMazeGenController_gen_eller(view->controller);
}

// Generate empty maze
static void h_gen_empty(GtkButton* widget, MgMazeGenView* view) {
    unused(widget);
    MgMazeGenController_gen_empty(view->controller);

}

// Rezise the maze
static void h_gen_crop(GtkButton* widget, MgMazeGenView* view) {
    unused(widget);
    MgMazeGenController_gen_crop(view->controller);
}