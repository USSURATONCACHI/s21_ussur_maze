#ifndef MAZEGTK_CONTROLLER_MAZE_GEN_H_
#define MAZEGTK_CONTROLLER_MAZE_GEN_H_

#include <stddef.h>
#include <stdint.h>
#include <libmaze/maze_struct.h>
#include <mazegtk/controller/maze.h>

// Opaque pointers are used instead of full defined structs to
// not encourage any usage of parts of inner implementation in view layer.
// This way, internals are simply inaccessible.
typedef struct MgMazeGenController MgMazeGenController;

MgMazeGenController* MgMazeGenController_new(MgMazeController* maze_controller); // depends on another controller
void MgMazeGenController_free(MgMazeGenController* controller);
void MgMazeGenController_update(MgMazeGenController* self);

size_t MgMazeGenController_how_much_ram_needed_mb(MgMazeGenController* self);
void MgMazeGenController_gen_random(MgMazeGenController* self);
void MgMazeGenController_gen_eller(MgMazeGenController* self);
void MgMazeGenController_gen_empty(MgMazeGenController* self);
void MgMazeGenController_gen_crop(MgMazeGenController* self);
void MgMazeGenController_cancel(MgMazeGenController* self);

size_t MgMazeGenController_get_gen_width (MgMazeGenController* self);
size_t MgMazeGenController_get_gen_height(MgMazeGenController* self);
void   MgMazeGenController_set_gen_width (MgMazeGenController* self, size_t new_width);
void   MgMazeGenController_set_gen_height(MgMazeGenController* self, size_t new_height);

bool MgMazeGenController_is_loading(MgMazeGenController* self);
void MgMazeGenController_join_generation_threads(MgMazeGenController* self);

#endif  // MAZEGTK_CONTROLLER_MAZE_GEN_H_