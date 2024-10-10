#include <mazegtk/controller/maze.h>
#include <better_c_std/camera/camera.h>
#include <better_c_std/prettify.h>

#include <libmaze/maze.h>

struct MgMazeController {
    MzMaze* maze;
    size_t maze_update_id;
    size_t last_maze_check_id;
    bool is_loading;
};

MgMazeController* MgMazeController_new(MzMaze* maze) {
    MgMazeController* controller = (void*) malloc(sizeof(MgMazeController));
    assert_alloc(controller);

    *controller = (MgMazeController) {
        .maze = maze,
        .maze_update_id = 1,
        .last_maze_check_id = 0,
    };

    return controller;
}
void MgMazeController_free(MgMazeController* controller) {
    free(controller);
}

void MgMazeController_set_maze(MgMazeController* self, MzMaze new_maze) {
    MzMaze_free(*self->maze);
    *self->maze = new_maze;
    self->maze_update_id++;
}
MzMaze* MgMazeController_get_maze(MgMazeController* self) {
    return self->maze;
}
bool MgMazeController_was_maze_updated(const MgMazeController* self) {
    return self->maze_update_id > self->last_maze_check_id;
}
void MgMazeController_maze_was_updated(MgMazeController* self) {
    self->last_maze_check_id = self->maze_update_id;
}
void MgMazeController_increment_update_id(MgMazeController* self) {
    self->maze_update_id++;
}

uint8_t* MgMazeController_data_buffer(MgMazeController* self) {
    return self->maze->raw_data;
}
size_t MgMazeController_data_size(MgMazeController* self) {
    return MzMaze_get_buffer_size(self->maze);
}
size_t MgMazeController_width(MgMazeController* self) {
    return self->maze->width;
}
size_t MgMazeController_height(MgMazeController* self) {
    return self->maze->height;
}


bool MgMazeController_is_loading(const MgMazeController* self) {
    return self->is_loading;
}
void MgMazeController_set_loading(MgMazeController* self, bool value) {
    self->is_loading = value;
}