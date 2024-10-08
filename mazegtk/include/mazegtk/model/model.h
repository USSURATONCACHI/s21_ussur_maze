#ifndef MAZEGTK_MODEL_MODEL_H_
#define MAZEGTK_MODEL_MODEL_H_

#include <better_c_std/camera/camera.h>
#include <libmaze/maze.h>

typedef struct {
    BcstdCamera camera;
    MzMaze maze;
} MgModel;

MgModel MgModel_new(size_t maze_width, size_t maze_height);
void MgModel_free(MgModel model);

void MgModel_generate_eller(MgModel* self, size_t width, size_t height);
void MgModel_generate_empty(MgModel* self, size_t width, size_t height);
void MgModel_crop_to_size  (MgModel* self, size_t width, size_t height);

void MgModel_reset_camera(MgModel* self);
void MgModel_reset_camera_settings(MgModel* self);

#endif // MAZEGTK_MODEL_MODEL_H_