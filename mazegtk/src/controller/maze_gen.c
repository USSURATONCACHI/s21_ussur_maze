#include <mazegtk/controller/maze_gen.h>
#include <better_c_std/prettify.h>
#include <libmaze/maze.h>

struct MgMazeGenController {
    MgMazeController* maze_ctl;
    size_t gen_width;
    size_t gen_height;
};

MgMazeGenController* MgMazeGenController_new(MgMazeController* maze_controller) {
    assert_m(maze_controller != NULL);

    MgMazeGenController* controller = (void*)malloc(sizeof(MgMazeGenController));
    assert_alloc(controller);

    *controller = (MgMazeGenController) {
        .maze_ctl = maze_controller,
        .gen_width = 10,
        .gen_height = 10,
    };

    return controller;
}

void MgMazeGenController_free(MgMazeGenController* controller) {
    free(controller);
}

// Methods

void MgMazeGenController_gen_eller(MgMazeGenController* self) {
    MzMazeResult res = mz_maze_generate_perfect_eller(self->gen_width, self->gen_height);
    if (res.is_ok) {
        MgMazeController_set_maze(self->maze_ctl, res.ok);
    } else {
        debugln("TODO: err %d", res.error);
    }
}
void MgMazeGenController_gen_empty(MgMazeGenController* self) {
    MzMazeResult res = mz_maze_create(self->gen_width, self->gen_height);

    if (res.is_ok) {
        MgMazeController_set_maze(self->maze_ctl, res.ok);
    } else {
        debugln("TODO: err %d", res.error);
    }

}
void MgMazeGenController_gen_crop(MgMazeGenController* self) {
    MzMaze* maze = MgMazeController_get_maze(self->maze_ctl);
    MzMazeResult res = mz_maze_crop_to_size(maze, self->gen_width, self->gen_height);

    if (res.is_ok) {
        MgMazeController_set_maze(self->maze_ctl, res.ok);
    } else {
        debugln("TODO: err %d", res.error);
    }
}

size_t MgMazeGenController_get_gen_width(MgMazeGenController* self) {
    return self->gen_width;
}
size_t MgMazeGenController_get_gen_height(MgMazeGenController* self) {
    return self->gen_height;
}
void MgMazeGenController_set_gen_width(MgMazeGenController* self, size_t new_width) {
    self->gen_width = new_width;  
}
void MgMazeGenController_set_gen_height(MgMazeGenController* self, size_t new_height) {
    self->gen_height = new_height;  
}