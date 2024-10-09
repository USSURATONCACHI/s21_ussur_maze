#ifndef MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_CONTROLS_VIEW_H_
#define MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_CONTROLS_VIEW_H_

#include <mazegtk/controller/camera.h>
#include <gtk/gtk.h>

typedef struct {
    MgCameraController* controller;

    GtkSpinButton* x_btn;
    GtkSpinButton* y_btn;
    GtkSpinButton* zoom_btn;
    GtkButton*     reset_btn;

    // GTK does not differ user input and programmatic `_set_value`. 
    // So, we compare input to last set value to check if it is set by the program or by the user.
    struct {
        gdouble cam_x;
        gdouble cam_y;
        gdouble zoom;
    } last_shown;
} MgCameraControlsView;

MgCameraControlsView* MgCameraControlsView_create(GtkBuilder* ui, MgCameraController* controller);
void MgCameraControlsView_free(MgCameraControlsView* view);

void MgCameraControlsView_update_ui(MgCameraControlsView* view);

#endif // MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_CONTROLS_VIEW_H_