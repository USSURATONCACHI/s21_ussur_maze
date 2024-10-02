#ifndef LIBMAZE_INCLUDE_MAZE_H_
#define LIBMAZE_INCLUDE_MAZE_H_

#include <stdbool.h>
#include <stddef.h>

#include <libmaze/maze_struct.h>
#include <libmaze/cell_struct.h>
#include <libmaze/directions_struct.h>

Maze maze_create(size_t width, size_t height);
void maze_free(Maze maze);

void maze_fill_random(Maze maze);

MazeCell maze_at(Maze maze, size_t x, size_t y);
void maze_set_at(Maze maze, size_t x, size_t y, MazeCell value);

MazeDirections maze_where_can_go(Maze maze, size_t x, size_t y);
void maze_print(Maze maze);

#endif // LIBMAZE_INCLUDE_MAZE_H_