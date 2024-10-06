#include <mazegtk/controller/controller.h>
#include <mazegtk/model/model.h>

#include <stdlib.h>

typedef struct {
    MgModel* model;
    MgCameraController* camera;

    vec_MzMaze maze_mipmaps;
} ControllerImpl;

MgController* MgController_new(MgModel* model_ref) {
    ControllerImpl* cont = (ControllerImpl*)malloc(sizeof(MgModel));
    
    cont->model = model_ref;
    cont->camera = MgCameraController_new(&model_ref->camera);
    cont->maze_mipmaps = mz_maze_generate_mipmaps(&model_ref->maze);

    return (MgController*) cont;
}
void MgController_free(MgController* controller) {
    ControllerImpl* cont = (void*) controller;
    MgCameraController_free(cont->camera);
    vec_MzMaze_free(cont->maze_mipmaps);
    free(cont);
}


MgCameraController* MgController_get_camera(MgController* self) {
    ControllerImpl* cont = (void*) self;
    return cont->camera;
}

size_t  MgController_get_maze_mipmaps_count(MgController* self) {
    ControllerImpl* cont = (void*) self;
    return cont->maze_mipmaps.length + 1; // +1 for original maze
}

MgMazeController* MgController_get_maze(MgController* self, size_t mipmap_level) {
    ControllerImpl* cont = (void*) self;
    if (mipmap_level == 0) {
        return MgMazeController_new(&cont->model->maze);
    } else {
        size_t mipmaps_count = MgController_get_maze_mipmaps_count(self);
        if (mipmap_level > mipmaps_count)
            mipmap_level = mipmaps_count;

        return MgMazeController_new(&cont->maze_mipmaps.data[mipmap_level - 1]);
    }
}