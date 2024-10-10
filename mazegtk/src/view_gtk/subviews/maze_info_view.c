#include <mazegtk/view_gtk/subviews/maze_info_view.h>
#include <mazegtk/view_gtk/subviews/ui_update_macros.h>
#include <better_c_std/prettify.h>

static void h_open_from_file_btn(GtkWidget* widget, MgMazeInfoView* view);
static void h_save_btn(GtkWidget* widget, MgMazeInfoView* view);
static void h_save_as_btn(GtkWidget* widget, MgMazeInfoView* view);

MgMazeInfoView* MgMazeInfoView_create(GtkBuilder* ui, MgMazeController* controller) {
    if (ui == NULL || controller == NULL)
        return NULL;

    MgMazeInfoView* view = (void*) malloc(sizeof(MgMazeInfoView));
    assert_alloc(view);

    *view = (MgMazeInfoView) {
        .controller = controller,


        .width_label        = GTK_LABEL(gtk_builder_get_object(ui, "width_label")),
        .height_label       = GTK_LABEL(gtk_builder_get_object(ui, "height_label")),
        .defects_label      = GTK_LABEL(gtk_builder_get_object(ui, "defects_label")),
        .open_from_file_btn = GTK_BUTTON(gtk_builder_get_object(ui, "open_from_file_btn")),
        .save_btn           = GTK_BUTTON(gtk_builder_get_object(ui, "save_btn")),
        .save_as_btn        = GTK_BUTTON(gtk_builder_get_object(ui, "save_as_btn")),

        .last_shown = {}, // can be uninitialized, does not matter 
    };

    if (view->open_from_file_btn) g_signal_connect(view->open_from_file_btn, "clicked", G_CALLBACK(h_open_from_file_btn), view);
    if (view->save_btn)           g_signal_connect(view->save_btn,           "clicked", G_CALLBACK(h_save_btn),           view);
    if (view->save_as_btn)        g_signal_connect(view->save_as_btn,        "clicked", G_CALLBACK(h_save_as_btn),        view);

    return view;
}

void MgMazeInfoView_free(MgMazeInfoView* view) {
    if (view == NULL)
        return;

    if (view->open_from_file_btn) g_signal_handlers_disconnect_by_data(view->open_from_file_btn, view);
    if (view->save_as_btn)        g_signal_handlers_disconnect_by_data(view->save_as_btn,        view);
    if (view->save_btn)           g_signal_handlers_disconnect_by_data(view->save_btn,           view);

    free(view);
}


void MgMazeInfoView_update_ui(MgMazeInfoView* view) {
    if (view == NULL)
        return;

    UPDATE_LABEL_SIZE_T(view->width_label, MgMazeController_width(view->controller));
    UPDATE_LABEL_SIZE_T(view->height_label, MgMazeController_height(view->controller));
}


static void h_open_from_file_btn(GtkWidget* widget, MgMazeInfoView* view) {
    unused(widget);
    unused(view);
    debugln("TODO");
}
static void h_save_btn(GtkWidget* widget, MgMazeInfoView* view) {
    unused(widget);
    unused(view);
    debugln("TODO");
}
static void h_save_as_btn(GtkWidget* widget, MgMazeInfoView* view) {
    unused(widget);
    unused(view);
    debugln("TODO");
}