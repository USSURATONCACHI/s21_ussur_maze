#include "better_c_std/prettify/assert.h"
#include <mazegtk/controller/camera.h>
#include <mazegtk/model/camera.h>
#include <better_c_std/prettify.h>

typedef struct {
    MgCameraModel* model;
} ControllerImpl;

MgCameraController* MgCameraController_new(void* model_ref) {
    ControllerImpl* cont = (ControllerImpl*) malloc(sizeof(ControllerImpl));
    assert_alloc(cont);

    cont->model = (MgCameraModel*) model_ref;
    return (MgCameraController*)cont;
}
void MgCameraController_free(MgCameraController* controller) {
    free(controller);
}

MgVector2 MgCameraController_pos(const MgCameraController* self_in) {
    const ControllerImpl* self = (void*)self_in;
    return MgCameraModel_pos(self->model);
}

MgVector2 MgCameraController_vel(const MgCameraController* self_in) {
    const ControllerImpl* self = (void*)self_in;
    return MgCameraModel_vel(self->model);
}

float MgCameraController_zoom(const MgCameraController* self_in) {
    const ControllerImpl* self = (void*)self_in;
    return MgCameraModel_zoom(self->model);
}

void MgCameraController_update_anim(MgCameraController* self_in) {
    const ControllerImpl* self = (void*)self_in;
    MgCameraModel_update_anim(self->model);
}


void MgCameraController_on_zoom(MgCameraController* self_in, float delta) {
    const ControllerImpl* self = (void*)self_in;
    MgCameraModel_on_zoom(self->model, delta);
}
void MgCameraController_on_drag_start(MgCameraController* self_in) {
    const ControllerImpl* self = (void*)self_in;
    MgCameraModel_on_drag_start(self->model);
}
void MgCameraController_on_drag(MgCameraController* self_in, MgVector2 drag) {
    const ControllerImpl* self = (void*)self_in;
    MgCameraModel_on_drag(self->model, drag);
}
void MgCameraController_on_drag_end(MgCameraController* self_in) {
    const ControllerImpl* self = (void*)self_in;
    MgCameraModel_on_drag_end(self->model);
}