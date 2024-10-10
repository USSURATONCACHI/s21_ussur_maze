#include <mazegtk/controller/maze_gen.h>
#include <better_c_std/prettify.h>
#include <libmaze/maze.h>
#include <pthread.h>
#include <string.h>

struct MgMazeGenController {
    MgMazeController* maze_ctl;
    size_t gen_width;
    size_t gen_height;

    pthread_t generation_thread;
    bool is_thread_running;
};

MgMazeGenController* MgMazeGenController_new(MgMazeController* maze_controller) {
    assert_m(maze_controller != NULL);

    MgMazeGenController* controller = (void*)malloc(sizeof(MgMazeGenController));
    assert_alloc(controller);

    *controller = (MgMazeGenController) {
        .maze_ctl = maze_controller,
        .gen_width = 10,
        .gen_height = 10,
        .is_thread_running = false,
    };

    return controller;
}

void MgMazeGenController_free(MgMazeGenController* controller) {
    free(controller);
}

// Methods
size_t MgMazeGenController_how_much_ram_needed_mb(MgMazeGenController* self) {
    return self->gen_width * self->gen_height * 2 / 8 / 1024 / 1024; // 2 bits per cell
}

static void thread_fn_gen_eller(MgMazeGenController* self) {
    {
        size_t w = self->gen_width;
        size_t h = self->gen_height;
        MzMaze* maze = MgMazeController_get_maze(self->maze_ctl);
        if (maze->width != w || maze->height != h) {
            MzMazeResult resized = mz_maze_create(w, h);
            if (!resized.is_ok) {
                debugln("TODO: Error: %d", resized.error);
            } else {
                MgMazeController_set_maze(self->maze_ctl, resized.ok);
            }
        }
    }

    MzMaze* maze = MgMazeController_get_maze(self->maze_ctl);
    memset(maze->raw_data, 0, mz_maze_get_buffer_size(maze)); // zero out the maze
    MzVoidResult res = mz_maze_fill_perfect_eller(maze);
    
    if (res.is_ok) {
        // nothing to do?
    } else {
        debugln("TODO: err %d", res.error);
    }
    MgMazeController_increment_update_id(self->maze_ctl);
    self->is_thread_running = false;
    MgMazeController_set_loading(self->maze_ctl, self->is_thread_running);
}

void MgMazeGenController_gen_eller(MgMazeGenController* self) {
    if (self->is_thread_running)
        return;

    self->is_thread_running = true;
    MgMazeController_set_loading(self->maze_ctl, self->is_thread_running);
    pthread_create(&self->generation_thread, NULL, (void*)thread_fn_gen_eller, self);
}

void MgMazeGenController_gen_random(MgMazeGenController* self) {
    if (self->is_thread_running)
        return;

    MzMazeResult res = mz_maze_create(self->gen_width, self->gen_height);
    if (res.is_ok) {
        mz_maze_fill_random(&res.ok);
        MgMazeController_set_maze(self->maze_ctl, res.ok);
    } else {
        debugln("TODO: err %d", res.error);
    }

}

void MgMazeGenController_gen_empty(MgMazeGenController* self) {
    if (self->is_thread_running)
        return;

    MzMazeResult res = mz_maze_create(self->gen_width, self->gen_height);

    if (res.is_ok) {
        MgMazeController_set_maze(self->maze_ctl, res.ok);
    } else {
        debugln("TODO: err %d", res.error);
    }

}
void MgMazeGenController_gen_crop(MgMazeGenController* self) {
    if (self->is_thread_running)
        return;

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

bool MgMazeGenController_is_loading(MgMazeGenController* self) {
    return MgMazeController_is_loading(self->maze_ctl);
}

bool MgMazeGenController_join_generation_threads(MgMazeGenController* self) {
    if (self->is_thread_running) {
        pthread_join(self->generation_thread, NULL);
    }
}