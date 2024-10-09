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


// static MgCreateError MzError_to_MgCreateError(MzError err) {
//     switch (err) {
//         case NOT_ENOUGH_MEMORY:
//             return MG_NOT_ENOUGH_MEMORY;
        
//         default:
//             panic("Unknown maze error type");
//     }
// }

// MgCreateError MgController_create_maze_empty(MgController* self, size_t maze_width, size_t maze_height) {
//     MzMazeResult result = mz_maze_create(maze_width, maze_height);

//     if (result.is_ok) {
//         mz_maze_free(self->model->maze);
//         self->model->maze = result.ok;
//         return MG_SUCCESS;
//     } else {
//         return MzError_to_MgCreateError(result.error);
//     }
// }
// MgCreateError MgController_create_maze_eller(MgController* self, size_t maze_width, size_t maze_height) {
//     MzMazeResult result = mz_maze_generate_perfect_eller(maze_width, maze_height);

//     if (result.is_ok) {
//         mz_maze_free(self->model->maze);
//         self->model->maze = result.ok;
//         return MG_SUCCESS;
//     } else {
//         return MzError_to_MgCreateError(result.error);
//     }
// }
// MgCreateError MgController_create_maze_random(MgController* self, size_t maze_width, size_t maze_height) {
//     MzMazeResult result = mz_maze_create(maze_width, maze_height);

//     if (result.is_ok) {
//         mz_maze_free(self->model->maze);
//         self->model->maze = result.ok;
//         mz_maze_fill_random(&self->model->maze);
//         return MG_SUCCESS;
//     } else {
//         return MzError_to_MgCreateError(result.error);
//     }
// }