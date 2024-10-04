#ifndef MAZEGTK_MODEL_CAMERA_H_
#define MAZEGTK_MODEL_CAMERA_H_

#include <mazegtk/model/vector2.h>

typedef struct {
    MgVector2 pos;
    MgVector2 vel;
    double vel_start_time;

    MgVector2 next_vel;
    double last_drag_time;
    double vel_exp;
    float vel_inertia;

    float zoom;
    float zoom_vel;
    double zoom_vel_start_time;
    double zoom_exp;
} MgCameraModel;

MgCameraModel MgCameraModel_new(float start_x, float start_y);

MgVector2  MgCameraModel_pos(const MgCameraModel* self);
MgVector2  MgCameraModel_vel(const MgCameraModel* self);
void       MgCameraModel_set_pos(MgCameraModel* self, MgVector2 new_pos);
void       MgCameraModel_wrap_x(MgCameraModel* self, float max);
void       MgCameraModel_wrap_y(MgCameraModel* self, float max);
void       MgCameraModel_update_anim(MgCameraModel* self);
float      MgCameraModel_zoom(const MgCameraModel* self);
float      MgCameraModel_zoom_vel(const MgCameraModel* self);
void       MgCameraModel_set_zoom(MgCameraModel* self, float new_zoom);
void       MgCameraModel_on_zoom(MgCameraModel* self, float delta);
void       MgCameraModel_on_drag(MgCameraModel* self, MgVector2 drag);
void       MgCameraModel_on_drag_start(MgCameraModel* self);
void       MgCameraModel_on_drag_end(MgCameraModel* self);

#endif // MAZEGTK_MODEL_CAMERA_H_