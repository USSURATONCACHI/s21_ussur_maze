#ifndef MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_MOUSE_VIEW_H_
#define MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_MOUSE_VIEW_H_

#include <better_c_std/time.h>
#include <mazegtk/controller/camera.h>
#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct {
    MgCameraController* controller;
    GtkGLArea* gl_area;
    GtkMenu* context_menu;

    BcstdTime lmb_pressed_timestamp;
    bool is_lmb_pressed;
    int prev_mouse_x, prev_mouse_y;
} MgCameraMouseView;

MgCameraMouseView* MgCameraMouseView_create(GtkBuilder* ui, MgCameraController* controller);
void MgCameraMouseView_free(MgCameraMouseView* view);

#endif // MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_MOUSE_VIEW_H_