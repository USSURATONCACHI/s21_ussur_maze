#ifndef MAZEGTK_CONTROLLER_CAMERA_H_
#define MAZEGTK_CONTROLLER_CAMERA_H_

#include <mazegtk/util/vector2.h>

typedef struct MgCameraController MgCameraController;

MgCameraController* MgCameraController_new(void* model_ref);
void MgCameraController_free(MgCameraController* controller);

MgVector2 MgCameraController_pos(const MgCameraController* self);
MgVector2 MgCameraController_vel(const MgCameraController* self);
float     MgCameraController_zoom(const MgCameraController* self);

void MgCameraController_set_pos(MgCameraController* self, MgVector2 pos);
void MgCameraController_set_zoom(MgCameraController* self, float zoom);
void MgCameraController_stop_movement(MgCameraController* self);

void MgCameraController_update_anim(MgCameraController* self);
void MgCameraController_on_zoom(MgCameraController* self, float delta);
void MgCameraController_on_drag_start(MgCameraController* self);
void MgCameraController_on_drag(MgCameraController* self, MgVector2 drag);
void MgCameraController_on_drag_end(MgCameraController* self);

#endif // MAZEGTK_CONTROLLER_CAMERA_H_