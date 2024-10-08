#include <mazegtk/controller/camera.h>
#include <better_c_std/camera/camera.h>
#include <better_c_std/prettify.h>


MgCameraController* MgCameraController_new(BcstdCamera* model_ref) {
    return (void*) model_ref;
}
void MgCameraController_free(MgCameraController* controller) {
    unused(controller);
}

// Getters
MgVector2 MgCameraController_get_pos(const MgCameraController* self) {
    const BcstdCamera* camera = (void*)self;
    BcstdVec3 vec = BcstdCamera_get_pos(camera);
    return (MgVector2) { vec.x, vec.y };
}
MgVector2 MgCameraController_get_vel(const MgCameraController* self) {
    const BcstdCamera* camera = (void*)self;
    BcstdVec3 vec = BcstdCamera_get_vel(camera);
    return (MgVector2) { vec.x, vec.y };
}
long double MgCameraController_get_zoom(const MgCameraController* self) {
    const BcstdCamera* camera = (void*)self;
    return BcstdCamera_get_zoom(camera);
}
long double MgCameraController_get_cell_size(const MgCameraController* self) {
    const BcstdCamera* camera = (void*)self;
    return BcstdCamera_get_unit_size(camera);
}
long double MgCameraController_get_drag_sens(const MgCameraController* self) {
    const BcstdCamera* camera = (void*)self;
    return (camera->drag_sensitivity.x + camera->drag_sensitivity.y) / 2.0;

}
long double MgCameraController_get_zoom_sens(const MgCameraController* self) {
    const BcstdCamera* camera = (void*)self;
    return camera->zoom_sensitivity;
}

// Setters
void MgCameraController_set_pos(MgCameraController* self, MgVector2 pos) {
    BcstdCamera* camera = (void*)self;
    BcstdCamera_set_pos(camera, (BcstdVec2) {
        .x = pos.x,
        .y = pos.y,
    });
}
void MgCameraController_set_zoom(MgCameraController* self, long double zoom) {
    BcstdCamera* camera = (void*)self;
    BcstdCamera_set_zoom(camera, zoom);
}
void MgCameraController_stop_movement(MgCameraController* self) {
    BcstdCamera* camera = (void*)self;
    BcstdCamera_set_vel(camera, (BcstdVec2) { 0, 0 });
    BcstdCamera_set_vel_zoom(camera, 0);
}

void MgCameraController_set_x(MgCameraController* self, long double x) {
    BcstdCamera* camera = (void*)self;
    BcstdCamera_set_x(camera, x);
}
void MgCameraController_set_y(MgCameraController* self, long double y) {
    BcstdCamera* camera = (void*)self;
    BcstdCamera_set_y(camera, y);
}
void MgCameraController_set_drag_sens(MgCameraController* self, long double s) {
    BcstdCamera* camera = (void*)self;
    camera->drag_sensitivity.x = s;
    camera->drag_sensitivity.y = s;
}
void MgCameraController_set_zoom_sens(MgCameraController* self, long double s) {
    BcstdCamera* camera = (void*)self;
    BcstdCamera_set_vel_zoom(camera, s);
}
void MgCameraController_reset_camera(MgCameraController* self) {
    unused(self);
    debugln("TODO");
}
void MgCameraController_reset_settings(MgCameraController* self) {
    unused(self);
    debugln("TODO");
}

// Functions to call on events
void MgCameraController_update_anim(MgCameraController* self) {
    BcstdCamera* camera = (void*)self;
    BcstdCamera_update_origin(camera);
}
void MgCameraController_on_zoom(MgCameraController* self, float delta) {
    BcstdCamera* camera = (void*)self;
    BcstdCamera_on_zoom(camera, delta);
}
void MgCameraController_on_drag_start(MgCameraController* self) {
    BcstdCamera* camera = (void*)self;
    BcstdCamera_on_drag_start(camera);
}
void MgCameraController_on_drag(MgCameraController* self, MgVector2 drag) {
    BcstdCamera* camera = (void*)self;
    BcstdCamera_on_drag(camera, (BcstdVec2) { .x = drag.x, .y = drag.y });
}
void MgCameraController_on_drag_end(MgCameraController* self) {
    BcstdCamera* camera = (void*)self;
    BcstdCamera_on_drag_end(camera);
}