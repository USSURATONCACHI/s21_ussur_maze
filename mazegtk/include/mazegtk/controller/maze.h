#ifndef MAZEGTK_CONTROLLER_MAZE_H_
#define MAZEGTK_CONTROLLER_MAZE_H_

#include <stddef.h>
#include <stdint.h>
#include <libmaze/maze_struct.h>

// Opaque pointers are used instead of full defined structs to
// not encourage any usage of parts of inner implementation in view layer.
// This way, internals are simply inaccessible.
typedef struct MgMazeController MgMazeController;

MgMazeController* MgMazeController_new(MzMaze* maze);
void MgMazeController_free(MgMazeController* controller);

bool MgMazeController_was_maze_updated(const MgMazeController* self);  // check if updated since last call to `MgMazeController_maze_was_updated`
void MgMazeController_maze_was_updated(MgMazeController* self);

uint8_t* MgMazeController_data_buffer(MgMazeController* self);
size_t   MgMazeController_data_size  (MgMazeController* self);
size_t   MgMazeController_width      (MgMazeController* self);
size_t   MgMazeController_height     (MgMazeController* self);

// For inter-controller use
void MgMazeController_set_maze(MgMazeController* self, MzMaze new_maze);
MzMaze* MgMazeController_get_maze(MgMazeController* self);

#endif // MAZEGTK_CONTROLLER_MAZE_H_