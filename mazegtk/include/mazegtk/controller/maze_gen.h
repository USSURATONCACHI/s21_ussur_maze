#ifndef MAZEGTK_CONTROLLER_MAZE_GEN_H_
#define MAZEGTK_CONTROLLER_MAZE_GEN_H_

#include <stddef.h>
#include <stdint.h>
#include <libmaze/maze_struct.h>
#include <mazegtk/controller/maze.h>

typedef struct MgMazeGenController MgMazeGenController;

MgMazeGenController* MgMazeGenController_new(MgMazeController* maze_controller); // depends on another controller
void MgMazeGenController_free(MgMazeGenController* controller);

void MgMazeGenController_gen_eller(MgMazeGenController* self);
void MgMazeGenController_gen_empty(MgMazeGenController* self);
void MgMazeGenController_gen_crop(MgMazeGenController* self);

size_t MgMazeGenController_get_gen_width (MgMazeGenController* self);
size_t MgMazeGenController_get_gen_height(MgMazeGenController* self);
void   MgMazeGenController_set_gen_width (MgMazeGenController* self, size_t new_width);
void   MgMazeGenController_set_gen_height(MgMazeGenController* self, size_t new_width);

#endif  // MAZEGTK_CONTROLLER_MAZE_GEN_H_