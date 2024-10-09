#ifndef MAZEGTK_VIEW_MAZE_GEN_VIEW_H_
#define MAZEGTK_VIEW_MAZE_GEN_VIEW_H_

#include <mazegtk/controller/maze_gen.h>
#include <gtk/gtk.h>

typedef struct {
    MgMazeGenController* controller;

    GtkSpinButton* gen_w_btn;
    GtkSpinButton* gen_h_btn;

    GtkButton* gen_eller_btn;
    GtkButton* gen_empty_btn;
    GtkButton* gen_crop_btn;

    // GTK does not differ user input and programmatic `_set_value`. 
    // So, we compare input to last set value to check if it is set by the program or by the user.
    struct {
        gdouble gen_w;
        gdouble gen_h;
    } last_shown;
} MgMazeGenView;

MgMazeGenView* MgMazeGenView_create(GtkBuilder* ui, MgMazeGenController* controller);
void MgMazeGenView_free(MgMazeGenView* view);

void MgMazeGenView_update_ui(MgMazeGenView* view);

#endif // MAZEGTK_VIEW_MAZE_GEN_VIEW_H_