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

void MgModel_reset_camera(MgModel* self);
void MgModel_reset_camera_settings(MgModel* self);

#endif // MAZEGTK_MODEL_MODEL_H_