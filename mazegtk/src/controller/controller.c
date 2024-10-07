#include <mazegtk/controller/controller.h>
#include <mazegtk/model/model.h>

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


void MgController_create_maze_empty(MgController* self, size_t maze_width, size_t maze_height) {
    mz_maze_free(self->model->maze);
    self->model->maze = mz_maze_create(maze_width, maze_height);
}
void MgController_create_maze_eller(MgController* self, size_t maze_width, size_t maze_height) {
    mz_maze_free(self->model->maze);
    self->model->maze = mz_maze_create(maze_width, maze_height);
    mz_maze_generate_perfect_eller(&self->model->maze);
}
void MgController_create_maze_random(MgController* self, size_t maze_width, size_t maze_height) {
    mz_maze_free(self->model->maze);
    self->model->maze = mz_maze_create(maze_width, maze_height);
    mz_maze_fill_random(&self->model->maze);
}