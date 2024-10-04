#ifndef MAZEGTK_CONTROLLER_CONTROLLER_H_
#define MAZEGTK_CONTROLLER_CONTROLLER_H_

#include <libmaze/maze_struct.h>
#include <mazegtk/controller/camera.h>
#include <mazegtk/model/model.h>

typedef struct MgController MgController;

MgController* MgController_new(MgModel* model_ref);
void MgController_free(MgController* controller);

MzMaze* MgController_get_maze(MgController* self);
MgCameraController* MgController_get_camera(MgController* self);


#endif // MAZEGTK_CONTROLLER_CONTROLLER_H_