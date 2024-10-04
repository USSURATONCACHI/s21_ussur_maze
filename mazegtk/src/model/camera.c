#include <mazegtk/model/camera.h>

#include <stdbool.h>
#include <float.h>
#include <time.h>
#include <math.h>

#define CAMERA_ZOOM_EXP 1000.0
#define CAMERA_VEL_EXP 128.0

static double current_time_secs() {
  static clock_t Start;
  static bool IsInit = false;

  clock_t now = clock();

  if (!IsInit) {
    Start = now;
    IsInit = true;
  }

  return ((double)(now - Start)) / CLOCKS_PER_SEC;
}

static float clamp(float x, float min, float max) {
  return x < min ? min : (x > max ? max : x);
}


MgCameraModel MgCameraModel_new(float start_x, float start_y) {
  double now = current_time_secs();

  MgCameraModel camera = {.pos = (MgVector2){start_x, start_y},
                       .vel = (MgVector2){0.0f, 0.0f},
                       .vel_start_time = now,
                       .next_vel = (MgVector2){0.0f, 0.0f},
                       .last_drag_time = now,
                       .vel_exp = CAMERA_VEL_EXP,
                       .vel_inertia = 0.3f,
                       .zoom = 20.0f,
                       .zoom_vel = 0.0f,
                       .zoom_vel_start_time = now,
                       .zoom_exp = CAMERA_ZOOM_EXP};

  return camera;
}

MgVector2 MgCameraModel_pos(const MgCameraModel* self) {
  double q = 1.0 / self->vel_exp;

  MgVector2 pos = self->pos;
  MgVector2 vel = self->vel;
  double t = current_time_secs() - self->vel_start_time;

  float coef = (float)((1.0 - pow(q, t)) / (1.0 - q));
  MgVector2 new_pos;
  new_pos.x = pos.x + self->vel_inertia * vel.x * coef;
  new_pos.y = pos.y + self->vel_inertia * vel.y * coef;

  new_pos.x = clamp(new_pos.x, -FLT_MAX, FLT_MAX);
  new_pos.y = clamp(new_pos.y, -FLT_MAX, FLT_MAX);

  return new_pos;
}

MgVector2 MgCameraModel_vel(const MgCameraModel* self) {
  double q = 1.0 / self->vel_exp;
  double t = current_time_secs() - self->zoom_vel_start_time;
  float coef = (float)pow(q, t);
  MgVector2 new_vel;
  new_vel.x = self->vel_inertia * self->vel.x * coef;
  new_vel.y = self->vel_inertia * self->vel.y * coef;
  return new_vel;
}

void MgCameraModel_set_pos(MgCameraModel* self, MgVector2 new_pos) {
  MgVector2 new_vel = MgCameraModel_vel(self);
  self->pos = new_pos;
  self->vel = new_vel;
  self->vel_start_time = current_time_secs();
}

void MgCameraModel_wrap_x(MgCameraModel* self, float max) {
  MgVector2 pos = MgCameraModel_pos(self);
  float new_x = fmodf((fmodf(pos.x, max) + max), max);
  float dx = new_x - pos.x;
  self->pos.x += dx;
}

void MgCameraModel_wrap_y(MgCameraModel* self, float max) {
  MgVector2 pos = MgCameraModel_pos(self);
  float new_y = fmodf((fmodf(pos.y, max) + max), max);
  float dy = new_y - pos.y;
  self->pos.y += dy;
}

void MgCameraModel_update_anim(MgCameraModel* self) {
  MgVector2 new_pos = MgCameraModel_pos(self);
  MgVector2 new_vel = MgCameraModel_vel(self);
  float new_zoom_vel = MgCameraModel_zoom_vel(self);
  double now = current_time_secs();

  self->pos = new_pos;
  self->vel = new_vel;
  self->zoom_vel = new_zoom_vel;
  self->vel_start_time = now;
  self->zoom_vel_start_time = now;
}

float MgCameraModel_zoom(const MgCameraModel* self) {
  double q = 1.0 / self->zoom_exp;
  double t = current_time_secs() - self->zoom_vel_start_time;
  return self->zoom + self->zoom_vel * ((1.0 - pow(q, t)) / (1.0 - q));
}

float MgCameraModel_zoom_vel(const MgCameraModel* self) {
  double q = 1.0 / self->zoom_exp;
  double t = current_time_secs() - self->zoom_vel_start_time;
  return self->zoom_vel * (float)pow(q, t);
}

void MgCameraModel_set_zoom(MgCameraModel* self, float new_zoom) {
  self->zoom = new_zoom;
  self->zoom_vel = 0.0f;
  self->zoom_vel_start_time = current_time_secs();
}

void MgCameraModel_on_zoom(MgCameraModel* self, float delta) {
  self->zoom = MgCameraModel_zoom(self);
  self->zoom_vel = MgCameraModel_zoom_vel(self) + delta;
  self->zoom_vel_start_time = current_time_secs();
}

void MgCameraModel_on_drag(MgCameraModel* self, MgVector2 drag) {
  self->pos.x += drag.x;
  self->pos.y += drag.y;

  self->pos.x = clamp(self->pos.x, -FLT_MAX, FLT_MAX);
  self->pos.y = clamp(self->pos.y, -FLT_MAX, FLT_MAX);

  double now = current_time_secs();
  float dt = (float)(now - self->last_drag_time);
  if (dt < 0.001) dt = 0.001;

  self->last_drag_time = now;
  MgVector2 this_drag_vel = {drag.x / dt, drag.y / dt};

  if (isnan(this_drag_vel.x)) this_drag_vel.x = 0.0;
  if (isinf(this_drag_vel.x))
    this_drag_vel.x = this_drag_vel.x < 0.0 ? -FLT_MAX : FLT_MAX;

  if (isnan(this_drag_vel.y)) this_drag_vel.y = 0.0;
  if (isinf(this_drag_vel.y))
    this_drag_vel.y = this_drag_vel.y < 0.0 ? -FLT_MAX : FLT_MAX;

  self->next_vel.x = self->next_vel.x * 0.2f + this_drag_vel.x * 0.8f;
  self->next_vel.y = self->next_vel.y * 0.2f + this_drag_vel.y * 0.8f;
}

void MgCameraModel_on_drag_start(MgCameraModel* self) {
  self->pos = MgCameraModel_pos(self);
  self->vel = (MgVector2){0.0f, 0.0f};
  self->vel_start_time = current_time_secs();
}

#define Clamp(a, min, max) (a) < (min) ? (min) : ((a) > (max) ? (max) : (a))
#define MaxDragTime 0.25

void MgCameraModel_on_drag_end(MgCameraModel* self) {
  double now = current_time_secs();
  self->pos = MgCameraModel_pos(self);
  double drag_coef =
      Clamp(MaxDragTime - (now - self->last_drag_time), 0.0, MaxDragTime) /
      MaxDragTime;
  self->vel.x = self->next_vel.x * drag_coef;
  self->vel.y = self->next_vel.y * drag_coef;
  self->next_vel = (MgVector2){0.0f, 0.0f};
  self->vel_start_time = current_time_secs();
}