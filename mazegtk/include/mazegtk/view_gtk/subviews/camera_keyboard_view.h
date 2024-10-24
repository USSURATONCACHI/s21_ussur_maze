#ifndef MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_KEYBOARD_VIEW_H_
#define MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_KEYBOARD_VIEW_H_

#include <mazegtk/controller/camera.h>
#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct {
    MgCameraController* controller;
    GtkWindow* main_window;

    struct {
        bool is_pressed;
        bool was_pressed;
    } w, a, s, d, r, f; // For keys: W, A, S, D, R, F
} MgCameraKeyboardView;

MgCameraKeyboardView* MgCameraKeyboardView_create(GtkBuilder* ui, MgCameraController* controller);
void MgCameraKeyboardView_free(MgCameraKeyboardView* view);

void MgCameraKeyboardView_update(MgCameraKeyboardView* view);

#endif // MAZEGTK_VIEW_GTK_SUBVIEWS_CAMERA_KEYBOARD_VIEW_H_