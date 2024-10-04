#ifndef MAZEGTK_MODEL_MODEL_H_
#define MAZEGTK_MODEL_MODEL_H_

#include <mazegtk/model/camera.h>
#include <libmaze/maze.h>

typedef struct {
    MgCameraModel camera;
    MzMaze maze;
} MgModel;

#endif // MAZEGTK_MODEL_MODEL_H_