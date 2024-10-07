#include <mazegtk/controller/camera.h>
#include <mazegtk/model/camera.h>
#include <better_c_std/prettify.h>


MgCameraController* MgCameraController_new(void* model_ref) {
    MgCameraModel* cont = (MgCameraModel*) model_ref;
    return (MgCameraController*)cont;
}
void MgCameraController_free(MgCameraController* controller) {
    // nothing
}

MgVector2 MgCameraController_pos(const MgCameraController* self_in) {
    const MgCameraModel* model = (void*)self_in;
    return MgCameraModel_pos(model);
}

MgVector2 MgCameraController_vel(const MgCameraController* self_in) {
    const MgCameraModel* model = (void*)self_in;
    return MgCameraModel_vel(model);
}

float MgCameraController_zoom(const MgCameraController* self_in) {
    const MgCameraModel* model = (void*)self_in;
    return MgCameraModel_zoom(model);
}


void MgCameraController_set_pos(MgCameraController* self_in, MgVector2 pos) {
    MgCameraModel* model = (void*)self_in;
    MgCameraModel_set_pos(model, pos);
}
void MgCameraController_set_zoom(MgCameraController* self_in, float zoom) {
    MgCameraModel* model = (void*)self_in;
    MgCameraModel_set_zoom(model, zoom);
}
void MgCameraController_stop_movement(MgCameraController* self_in) {
    MgCameraModel* model = (void*)self_in;
    MgCameraModel_update_anim(model);
    model->vel.x = 0;
    model->vel.y = 0;
    model->zoom_vel = 0;
}


void MgCameraController_update_anim(MgCameraController* self_in) {
    MgCameraModel* model = (void*)self_in;
    MgCameraModel_update_anim(model);
}


void MgCameraController_on_zoom(MgCameraController* self_in, float delta) {
    MgCameraModel* model = (void*)self_in;
    MgCameraModel_on_zoom(model, delta);
}
void MgCameraController_on_drag_start(MgCameraController* self_in) {
    MgCameraModel* model = (void*)self_in;
    MgCameraModel_on_drag_start(model);
}
void MgCameraController_on_drag(MgCameraController* self_in, MgVector2 drag) {
    MgCameraModel* model = (void*)self_in;
    MgCameraModel_on_drag(model, drag);
}
void MgCameraController_on_drag_end(MgCameraController* self_in) {
    MgCameraModel* model = (void*)self_in;
    MgCameraModel_on_drag_end(model);
}