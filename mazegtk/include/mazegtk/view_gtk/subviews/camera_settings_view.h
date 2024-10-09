#ifndef MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_SETTINGS_VIEW_H_
#define MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_SETTINGS_VIEW_H_

#include <mazegtk/controller/camera.h>
#include <gtk/gtk.h>

typedef struct {
    MgCameraController* controller;

    GtkSpinButton* drag_sensitivity_btn;
    GtkSpinButton* zoom_speed_btn;
    GtkButton*     reset_btn;

    // GTK does not differ user input and programmatic `_set_value`. 
    // So, we compare input to last set value to check if it is set by the program or by the user.
    struct {
        gdouble drag_sensitivity;
        gdouble zoom_speed;
    } last_shown;
} MgCameraSettingsView;

MgCameraSettingsView* MgCameraSettingsView_create(GtkBuilder* ui, MgCameraController* controller);
void MgCameraSettingsView_free(MgCameraSettingsView* view);

void MgCameraSettingsView_update_ui(MgCameraSettingsView* view);

#endif // MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_SETTINGS_VIEW_H_