#include <mazegtk/controller/controller.h>
#include <mazegtk/model/model.h>
#include <better_c_std/prettify.h>

#include <stdlib.h>

struct MgController {
    MgModel* model;

    MgMazeController*    maze_controller;
    MgCameraController*  camera_controller;
    MgMazeGenController* maze_gen_controller;
};


MgController* MgController_new(MgModel* model_ref) {
    MgController* cont = (MgController*)malloc(sizeof(MgModel));
    assert_alloc(cont);

    *cont = (MgController) {
        .model = model_ref,
        .camera_controller   = MgCameraController_new(&model_ref->camera, model_ref),
        .maze_controller     = MgMazeController_new(&model_ref->maze),
        .maze_gen_controller = NULL,
    };

    if (cont->maze_controller)
        cont->maze_gen_controller = MgMazeGenController_new(cont->maze_controller);

    return (MgController*) cont;
}
void MgController_free(MgController* cont) {
    MgCameraController_free(cont->camera_controller);
    MgMazeController_free(cont->maze_controller);
    MgMazeGenController_free(cont->maze_gen_controller);
    free(cont);
}


MgCameraController* MgController_get_camera(MgController* self) {
    return self->camera_controller;
}
MgMazeGenController* MgController_get_maze_gen(MgController* self) {
    return self->maze_gen_controller;
}

MgMazeController* MgController_get_maze(MgController* self) {
    return self->maze_controller;
}