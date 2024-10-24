#ifndef MAZEGTK_CONTROLLER_CAMERA_H_
#define MAZEGTK_CONTROLLER_CAMERA_H_

// #include <better_c_std/camera/vec23.h>
#include <better_c_std/camera/camera.h>
#include <mazegtk/model/model.h>
#include <mazegtk/util/vector2.h>

// Opaque pointers are used instead of full defined structs to
// not encourage any usage of parts of inner implementation in view layer.
// This way, internals are simply inaccessible.
typedef struct MgCameraController MgCameraController;

// `MgModel*` is needed purely to reset settings when requested
MgCameraController* MgCameraController_new(BcstdCamera* camera, MgModel* model);
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
void MgCameraController_set_vel      (MgCameraController* self, MgVector2 vel);
void MgCameraController_set_zoom     (MgCameraController* self, long double zoom);
void MgCameraController_stop_movement(MgCameraController* self);

void MgCameraController_set_x         (MgCameraController* self, long double x);
void MgCameraController_set_y         (MgCameraController* self, long double y);
void MgCameraController_set_drag_sens (MgCameraController* self, long double s);
void MgCameraController_set_zoom_sens (MgCameraController* self, long double s);
void MgCameraController_reset_camera  (MgCameraController* self);
void MgCameraController_reset_settings(MgCameraController* self);

// Event handlers
void MgCameraController_update_anim  (MgCameraController* self); // Doubtful if even needed. Maybe will be removed later
void MgCameraController_on_zoom      (MgCameraController* self, float delta);
void MgCameraController_on_drag_start(MgCameraController* self);
void MgCameraController_on_drag      (MgCameraController* self, MgVector2 drag);
void MgCameraController_on_drag_end  (MgCameraController* self);

#endif // MAZEGTK_CONTROLLER_CAMERA_H_