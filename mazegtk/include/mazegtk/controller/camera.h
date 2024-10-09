#ifndef MAZEGTK_CONTROLLER_CAMERA_H_
#define MAZEGTK_CONTROLLER_CAMERA_H_

// #include <better_c_std/camera/vec23.h>
#include <better_c_std/camera/camera.h>
#include <mazegtk/util/vector2.h>

// Opaque pointers are used instead of full defined structs to
// not encourage coders (me) to use parts of inner implementation in view layer.
// This way, internals are simply inaccessible.
typedef struct MgCameraController MgCameraController;

MgCameraController* MgCameraController_new(BcstdCamera* model_ref);
void MgCameraController_free(MgCameraController* controller);

// Getters
MgVector2 MgCameraController_get_pos(const MgCameraController* self);
MgVector2 MgCameraController_get_vel(const MgCameraController* self);
long double MgCameraController_get_zoom(const MgCameraController* self);
long double MgCameraController_get_cell_size(const MgCameraController* self);
long double MgCameraController_get_drag_sens(const MgCameraController* self);
long double MgCameraController_get_zoom_sens(const MgCameraController* self);

// Setters
void MgCameraController_set_pos      (MgCameraController* self, MgVector2 pos);
void MgCameraController_set_zoom     (MgCameraController* self, long double zoom);
void MgCameraController_stop_movement(MgCameraController* self);

void MgCameraController_set_x         (MgCameraController* self, long double x);
void MgCameraController_set_y         (MgCameraController* self, long double y);
void MgCameraController_set_drag_sens (MgCameraController* self, long double s);
void MgCameraController_set_zoom_sens (MgCameraController* self, long double s);
void MgCameraController_reset_camera  (MgCameraController* self);
void MgCameraController_reset_settings(MgCameraController* self);

// Functions to call on events
void MgCameraController_update_anim  (MgCameraController* self);
void MgCameraController_on_zoom      (MgCameraController* self, float delta);
void MgCameraController_on_drag_start(MgCameraController* self);
void MgCameraController_on_drag      (MgCameraController* self, MgVector2 drag);
void MgCameraController_on_drag_end  (MgCameraController* self);

#endif // MAZEGTK_CONTROLLER_CAMERA_H_