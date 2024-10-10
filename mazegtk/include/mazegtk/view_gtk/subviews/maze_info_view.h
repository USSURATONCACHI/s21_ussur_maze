#ifndef MAZEGTK_VIEW_GTK_SUBVIEWS_MAZE_INFO_VIEW_H_
#define MAZEGTK_VIEW_GTK_SUBVIEWS_MAZE_INFO_VIEW_H_

#include <mazegtk/controller/maze.h>
#include <gtk/gtk.h>

typedef struct {
    MgMazeController* controller;

    GtkLabel* width_label;
    GtkLabel* height_label;
    GtkLabel* defects_label;

    GtkButton* open_from_file_btn;
    GtkButton* save_btn;
    GtkButton* save_as_btn;

    // GTK does not differ user input and programmatic `_set_value`. 
    // So, we compare input to last set value to check if it is set by the program or by the user.
    struct {
        size_t width;
        size_t height;
        size_t defects;
    } last_shown;
} MgMazeInfoView;

MgMazeInfoView* MgMazeInfoView_create(GtkBuilder* ui, MgMazeController* controller);
void MgMazeInfoView_free(MgMazeInfoView* view);

void MgMazeInfoView_update_ui(MgMazeInfoView* view);

#endif // MAZEGTK_VIEW_GTK_SUBVIEWS_MAZE_INFO_VIEW_H_