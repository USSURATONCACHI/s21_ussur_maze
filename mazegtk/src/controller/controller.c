#include <mazegtk/controller/controller.h>
#include <mazegtk/model/model.h>
#include <better_c_std/prettify.h>

#include <stdlib.h>

struct MgController {
    MgModel* model;
    MgCameraController* camera;
};


MgController* MgController_new(MgModel* model_ref) {
    MgController* cont = (MgController*)malloc(sizeof(MgModel));
    assert_alloc(cont);

    cont->model = model_ref;
    cont->camera = MgCameraController_new(&model_ref->camera);

    return (MgController*) cont;
}
void MgController_free(MgController* cont) {
    MgCameraController_free(cont->camera);
    free(cont);
}


MgCameraController* MgController_get_camera(MgController* self) {
    return self->camera;
}

MgMazeController* MgController_get_maze(MgController* self) {
    return MgMazeController_new(&self->model->maze);
}


static MgCreateError MzError_to_MgCreateError(MzError err) {
    switch (err) {
        case NOT_ENOUGH_MEMORY:
            return MG_NOT_ENOUGH_MEMORY;
        
        default:
            panic("Unknown maze error type");
    }
}

MgCreateError MgController_create_maze_empty(MgController* self, size_t maze_width, size_t maze_height) {
    MzMazeResult result = mz_maze_create(maze_width, maze_height);

    if (result.is_ok) {
        mz_maze_free(self->model->maze);
        self->model->maze = result.ok;
        return MG_SUCCESS;
    } else {
        return MzError_to_MgCreateError(result.error);
    }
}
MgCreateError MgController_create_maze_eller(MgController* self, size_t maze_width, size_t maze_height) {
    MzMazeResult result = mz_maze_generate_perfect_eller(maze_width, maze_height);

    if (result.is_ok) {
        mz_maze_free(self->model->maze);
        self->model->maze = result.ok;
        return MG_SUCCESS;
    } else {
        return MzError_to_MgCreateError(result.error);
    }
}
MgCreateError MgController_create_maze_random(MgController* self, size_t maze_width, size_t maze_height) {
    MzMazeResult result = mz_maze_create(maze_width, maze_height);

    if (result.is_ok) {
        mz_maze_free(self->model->maze);
        self->model->maze = result.ok;
        mz_maze_fill_random(&self->model->maze);
        return MG_SUCCESS;
    } else {
        return MzError_to_MgCreateError(result.error);
    }
}