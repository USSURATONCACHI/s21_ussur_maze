#include <mazegtk/controller/controller.h>
#include <mazegtk/model/model.h>

#include <stdlib.h>

typedef struct {
    MgModel* model;
    MgCameraController* camera;
} ControllerImpl;

MgController* MgController_new(MgModel* model_ref) {
    ControllerImpl* cont = (ControllerImpl*)malloc(sizeof(MgModel));
    
    cont->model = model_ref;
    cont->camera = MgCameraController_new(&model_ref->camera);

    return (MgController*) cont;
}
void MgController_free(MgController* controller) {
    ControllerImpl* cont = (void*) controller;
    MgCameraController_free(cont->camera);
    free(cont);
}

MzMaze* MgController_get_maze(MgController* self) {
}

MgCameraController* MgController_get_camera(MgController* self) {
    ControllerImpl* cont = (void*) self;
    return cont->camera;
}


uint8_t* MgController_get_maze_data_buffer(MgController* self) {
    ControllerImpl* cont = (void*) self;
    return cont->model->maze.raw_data;
}

size_t MgController_get_maze_data_size(MgController* self) {
    ControllerImpl* cont = (void*) self;
    return mz_maze_get_buffer_size(&cont->model->maze);
}
size_t MgController_get_maze_width(MgController* self) {
    ControllerImpl* cont = (void*) self;
    return cont->model->maze.width;
}
size_t MgController_get_maze_height(MgController* self) {
    ControllerImpl* cont = (void*) self;
    return cont->model->maze.height;
}