#ifndef MAZEGTK_CONTROLLER_CONTROLLER_H_
#define MAZEGTK_CONTROLLER_CONTROLLER_H_

#include <stdint.h>
#include <mazegtk/controller/camera.h>
#include <mazegtk/model/model.h>

typedef struct MgController MgController;

MgController* MgController_new(MgModel* model_ref);
void MgController_free(MgController* controller);

uint8_t* MgController_get_maze_data_buffer(MgController* self);
size_t MgController_get_maze_data_size(MgController* self);
size_t MgController_get_maze_width(MgController* self);
size_t MgController_get_maze_height(MgController* self);

MgCameraController* MgController_get_camera(MgController* self);


#endif // MAZEGTK_CONTROLLER_CONTROLLER_H_