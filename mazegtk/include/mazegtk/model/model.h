#ifndef MAZEGTK_MODEL_MODEL_H_
#define MAZEGTK_MODEL_MODEL_H_

#include <mazegtk/model/camera.h>
#include <libmaze/maze.h>

typedef struct {
    MgCameraModel camera;
    MzMaze maze;
} MgModel;

MgModel MgModel_new(size_t maze_width, size_t maze_height);
void MgModel_free(MgModel model);

#endif // MAZEGTK_MODEL_MODEL_H_