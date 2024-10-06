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


MgCameraController* MgController_get_camera(MgController* self) {
    ControllerImpl* cont = (void*) self;
    return cont->camera;
}

size_t  MgController_get_maze_mipmaps_count(MgController* self) {
    ControllerImpl* cont = (void*) self;
    return 1; // +1 for original maze
}

MgMazeController* MgController_get_maze(MgController* self, size_t mipmap_level) {
    ControllerImpl* cont = (void*) self;
    return MgMazeController_new(&cont->model->maze);
}