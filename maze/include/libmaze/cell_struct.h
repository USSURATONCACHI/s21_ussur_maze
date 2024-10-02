#ifndef LIBMAZE_INCLUDE_MAZE_CELL_STRUCT_H_
#define LIBMAZE_INCLUDE_MAZE_CELL_STRUCT_H_

#include <stdbool.h>

#define MAZE_CELL_SIZE_BITS 2
#define MAZE_CELL_BITMASK(type, pos) (((type) 3) << pos)
typedef struct {
    bool top_wall : 1;
    bool left_wall : 1;
} MazeCell;

#endif // LIBMAZE_INCLUDE_MAZE_CELL_STRUCT_H_