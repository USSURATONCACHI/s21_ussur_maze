#include <mazegtk/controller/camera.h>
#include <mazegtk/model/model.h>
#include <better_c_std/camera/camera.h>
#include <better_c_std/prettify.h>

struct MgCameraController {
    BcstdCamera* camera;
    MgModel* model;
};

MgCameraController* MgCameraController_new(BcstdCamera* camera, MgModel* model) {
    MgCameraController* controller = (void*) malloc(sizeof(MgCameraController));
    assert_alloc(controller);

    *controller = (MgCameraController){
        .camera = camera,
        .model = model,
    };

    return controller;
}
void MgCameraController_free(MgCameraController* controller) {
    free(controller);
}

// Getters
MgVector2 MgCameraController_get_pos(const MgCameraController* self) {
    BcstdVec3 vec = BcstdCamera_get_pos(self->camera);
    return (MgVector2) { vec.x, vec.y };
}
MgVector2 MgCameraController_get_vel(const MgCameraController* self) {
    BcstdVec3 vec = BcstdCamera_get_vel(self->camera);
    return (MgVector2) { vec.x, vec.y };
}
long double MgCameraController_get_zoom(const MgCameraController* self) {
    return BcstdCamera_get_zoom(self->camera);
}
long double MgCameraController_get_cell_size(const MgCameraController* self) {
    return BcstdCamera_get_unit_size(self->camera);
}
long double MgCameraController_get_drag_sens(const MgCameraController* self) {
    return (self->camera->drag_sensitivity.x + self->camera->drag_sensitivity.y) / 2.0;

}
long double MgCameraController_get_zoom_sens(const MgCameraController* self) {
    return self->camera->zoom_sensitivity;
}

// Setters
void MgCameraController_set_pos(MgCameraController* self, MgVector2 pos) {
    BcstdCamera_set_pos(self->camera, (BcstdVec2) {
        .x = pos.x,
        .y = pos.y,
    });
}
void MgCameraController_set_zoom(MgCameraController* self, long double zoom) {
    BcstdCamera_set_zoom(self->camera, zoom);
}
void MgCameraController_stop_movement(MgCameraController* self) {
    BcstdCamera_set_vel(self->camera, (BcstdVec2) { 0, 0 });
    BcstdCamera_set_vel_zoom(self->camera, 0);
}

void MgCameraController_set_x(MgCameraController* self, long double x) {
    BcstdCamera_set_x(self->camera, x);
}
void MgCameraController_set_y(MgCameraController* self, long double y) {
    BcstdCamera_set_y(self->camera, y);
}
void MgCameraController_set_drag_sens(MgCameraController* self, long double s) {
    self->camera->drag_sensitivity.x = s;
    self->camera->drag_sensitivity.y = s;
}
void MgCameraController_set_zoom_sens(MgCameraController* self, long double s) {
    self->camera->zoom_sensitivity = s;
}
void MgCameraController_reset_camera(MgCameraController* self) {
    if (self->model)
        MgModel_reset_camera(self->model);
}
void MgCameraController_reset_settings(MgCameraController* self) {
    if (self->model)
        MgModel_reset_camera_settings(self->model);
}

// Functions to call on events
void MgCameraController_update_anim(MgCameraController* self) {
    BcstdCamera_update_origin(self->camera);
}
void MgCameraController_on_zoom(MgCameraController* self, float delta) {
    BcstdCamera_on_zoom(self->camera, delta);
}
void MgCameraController_on_drag_start(MgCameraController* self) {
    BcstdCamera_on_drag_start(self->camera);
}
void MgCameraController_on_drag(MgCameraController* self, MgVector2 drag) {
    BcstdCamera_on_drag(self->camera, (BcstdVec2) { .x = drag.x, .y = drag.y });
}
void MgCameraController_on_drag_end(MgCameraController* self) {
    BcstdCamera_on_drag_end(self->camera);
}