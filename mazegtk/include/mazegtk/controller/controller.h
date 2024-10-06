#ifndef MAZEGTK_CONTROLLER_CONTROLLER_H_
#define MAZEGTK_CONTROLLER_CONTROLLER_H_

#include <stdint.h>
#include <mazegtk/controller/camera.h>
#include <mazegtk/controller/maze.h>
#include <mazegtk/model/model.h>

typedef struct MgController MgController;

MgController* MgController_new(MgModel* model_ref);
void MgController_free(MgController* controller);

size_t  MgController_get_maze_mipmaps_count(MgController* self);

MgMazeController* MgController_get_maze(MgController* self, size_t mipmap_level);

MgCameraController* MgController_get_camera(MgController* self);


#endif // MAZEGTK_CONTROLLER_CONTROLLER_H_