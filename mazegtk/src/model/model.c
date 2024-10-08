#include <mazegtk/model/model.h>
#include <time.h>
#include <stdlib.h>
#include <better_c_std/prettify.h>
#include <libmaze/maze.h>

MgModel MgModel_new(size_t maze_width, size_t maze_height) {
    srand(time(NULL));
    MzMazeResult res = MzMaze_create_perfect_eller(maze_width, maze_height);
    assert_m(res.is_ok);

    MgModel model = {
        .maze = res.ok,
        .camera = BcstdCamera_create(),
    };
    MgModel_reset_camera_settings(&model);
    MgModel_reset_camera(&model);
    return model;
}
void MgModel_free(MgModel model) {
    MzMaze_free(model.maze);
}

void MgModel_reset_camera(MgModel* self) {
    BcstdCamera_set_vel(&self->camera, (BcstdVec2) { 0.0, 0.0 });
    BcstdCamera_set_vel_zoom(&self->camera, 0.0);

    BcstdCamera_set_pos(&self->camera, (BcstdVec2) { 
        self->maze.width / 2.0, 
        self->maze.height / 2.0 
    });
    BcstdCamera_set_zoom(&self->camera, 4.0);
}

void MgModel_reset_camera_settings(MgModel* self) {
    self->camera.drag_sensitivity = (BcstdVec2) { 1.0, 1.0 };
    self->camera.zoom_sensitivity = 4.0;
}
