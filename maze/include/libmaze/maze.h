#ifndef LIBMAZE_MAZE_H_
#define LIBMAZE_MAZE_H_

#include <stdbool.h>
#include <stddef.h>

#include <libmaze/maze_struct.h>
#include <libmaze/cell_struct.h>
#include <libmaze/directions_struct.h>

MzMaze mz_maze_create(size_t width, size_t height);
void   mz_maze_free(MzMaze maze);
void   mz_maze_fill_random(MzMaze* maze);

MzCell mz_maze_at(const MzMaze* maze, size_t x, size_t y);
void   mz_maze_set_at(MzMaze* maze, size_t x, size_t y, MzCell value);
void   mz_maze_print(const MzMaze* maze);

MzDirections mz_maze_where_can_go(const MzMaze* maze, size_t x, size_t y);

#endif // LIBMAZE_MAZE_H_