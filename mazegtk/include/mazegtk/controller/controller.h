#ifndef MAZEGTK_CONTROLLER_CONTROLLER_H_
#define MAZEGTK_CONTROLLER_CONTROLLER_H_

#include <stdint.h>
#include <mazegtk/controller/camera.h>
#include <mazegtk/controller/maze.h>
#include <mazegtk/model/model.h>

typedef struct MgController MgController;

MgController* MgController_new(MgModel* model_ref);
void MgController_free(MgController* controller);

MgMazeController* MgController_get_maze(MgController* self);
MgCameraController* MgController_get_camera(MgController* self);

void MgController_create_maze_empty(MgController* self, size_t maze_width, size_t maze_height);
void MgController_create_maze_eller(MgController* self, size_t maze_width, size_t maze_height);
void MgController_create_maze_random(MgController* self, size_t maze_width, size_t maze_height);


#endif // MAZEGTK_CONTROLLER_CONTROLLER_H_