#include <mazegtk/controller/maze_gen.h>

#include <better_c_std/prettify.h>
#include <libmaze/maze.h>
#include <libmaze/ellers_offthread_generator.h>

#include <string.h>

struct MgMazeGenController {
    MgMazeController* maze_ctl;
    MzEllersOffthreadGenerator* generator;
    bool generation_issued;

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
        .generator = MzEllersOffthreadGenerator_create(),
        .generation_issued = false,
    };

    return controller;
}

void MgMazeGenController_free(MgMazeGenController* controller) {
    MzEllersOffthreadGenerator_free(controller->generator);
    free(controller);
}

// Methods
void MgMazeGenController_update(MgMazeGenController* self) {
    if (self->generation_issued) {
        if (MzEllersOffthreadGenerator_is_finished(self->generator)) {
            MgMazeController_set_loading(self->maze_ctl, false);
            self->generation_issued = false;
        } else {
            MgMazeController_increment_update_id(self->maze_ctl);
        }
    }
}

size_t MgMazeGenController_how_much_ram_needed_mb(MgMazeGenController* self) {
    size_t size_bits = self->gen_width * self->gen_height * 2; // 2 bits per cell
    size_t size_mb = size_bits / 8 / 1024 / 1024;
    return  size_mb;
}

void MgMazeGenController_gen_eller(MgMazeGenController* self) {
    if (MgMazeGenController_is_loading(self))
        return;

    MgMazeGenController_gen_empty(self);
    MzEllersOffthreadGenerator_start(self->generator, MgMazeController_get_maze(self->maze_ctl));
    self->generation_issued = true;
    MgMazeController_set_loading(self->maze_ctl, true);
}

void MgMazeGenController_gen_random(MgMazeGenController* self) {
    if (MgMazeGenController_is_loading(self))
        return;

    MzMazeResult res = MzMaze_create(self->gen_width, self->gen_height);
    if (res.is_ok) {
        MzMaze_fill_random(&res.ok);
        MgMazeController_set_maze(self->maze_ctl, res.ok);
    } else {
        debugln("TODO: err %d", res.error);
    }

}

void MgMazeGenController_gen_empty(MgMazeGenController* self) {
    if (MgMazeGenController_is_loading(self))
        return;

    MzMazeResult res = MzMaze_create(self->gen_width, self->gen_height);

    if (res.is_ok) {
        MgMazeController_set_maze(self->maze_ctl, res.ok);
    } else {
        debugln("TODO: err %d", res.error);
    }

}
void MgMazeGenController_gen_crop(MgMazeGenController* self) {
    if (MgMazeGenController_is_loading(self))
        return;

    MzMaze* maze = MgMazeController_get_maze(self->maze_ctl);
    MzMazeResult res = MzMaze_crop_to_size(maze, self->gen_width, self->gen_height);

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

bool MgMazeGenController_is_loading(MgMazeGenController* self) {
    return !MzEllersOffthreadGenerator_is_finished(self->generator);
}

void MgMazeGenController_join_generation_threads(MgMazeGenController* self) {
    MzEllersOffthreadGenerator_await(self->generator);
}
void MgMazeGenController_cancel(MgMazeGenController* self) {
    if (!MzEllersOffthreadGenerator_is_finished(self->generator))
        MzEllersOffthreadGenerator_terminate(self->generator);
}