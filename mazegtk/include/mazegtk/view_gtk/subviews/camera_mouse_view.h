#ifndef MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_MOUSE_VIEW_H_
#define MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_MOUSE_VIEW_H_

#include <mazegtk/controller/camera.h>
#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct {
    MgCameraController* controller;
    GtkGLArea* gl_area;

    bool is_dragging;
    int prev_mouse_x, prev_mouse_y;
} MgCameraMouseView;

MgCameraMouseView* MgCameraMouseView_create(GtkBuilder* ui, MgCameraController* controller);
void MgCameraMouseView_free(MgCameraMouseView* view);

#endif // MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_MOUSE_VIEW_H_