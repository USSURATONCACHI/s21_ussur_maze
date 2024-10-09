#include <mazegtk/controller/maze_gen.h>
#include <better_c_std/prettify.h>
#include <libmaze/maze.h>

struct MgMazeGenController {
    MzMaze* maze;
    size_t gen_width;
    size_t gen_height;
};

MgMazeGenController* MgMazeGenController_new(MzMaze* maze) {
    assert_m(maze != NULL);

    MgMazeGenController* controller = (void*)malloc(sizeof(MgMazeGenController));
    assert_alloc(controller);

    *controller = (MgMazeGenController) {
        .maze = maze,
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
        mz_maze_free(*self->maze);
        *self->maze = res.ok;
    } else {
        debugln("TODO: err %d", res.error);
    }
}
void MgMazeGenController_gen_empty(MgMazeGenController* self) {
    MzMazeResult res = mz_maze_create(self->gen_width, self->gen_height);

    if (res.is_ok) {
        mz_maze_free(*self->maze);
        *self->maze = res.ok;
    } else {
        debugln("TODO: err %d", res.error);
    }

}
void MgMazeGenController_gen_crop(MgMazeGenController* self) {
    MzMazeResult res = mz_maze_crop_to_size(self->maze, self->gen_width, self->gen_height);

    if (res.is_ok) {
        mz_maze_free(*self->maze);
        *self->maze = res.ok;
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