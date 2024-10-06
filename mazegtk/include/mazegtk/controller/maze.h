#ifndef MAZEGTK_CONTROLLER_MAZE_H_
#define MAZEGTK_CONTROLLER_MAZE_H_

#include <stddef.h>
#include <stdint.h>

typedef struct MgMazeController MgMazeController;

MgMazeController* MgMazeController_new(void* maze);
void MgMazeController_free(MgMazeController* controller);

uint8_t* MgMazeController_data_buffer(MgMazeController* self);
size_t   MgMazeController_data_size  (MgMazeController* self);
size_t   MgMazeController_width      (MgMazeController* self);
size_t   MgMazeController_height     (MgMazeController* self);

#endif // MAZEGTK_CONTROLLER_MAZE_H_