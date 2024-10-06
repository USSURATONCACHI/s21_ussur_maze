#ifndef LIBMAZE_MAZE_STRUCT_H_
#define LIBMAZE_MAZE_STRUCT_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t* raw_data;
    size_t width;
    size_t height;
} MzMaze;

#define VECTOR_H MzMaze
#include <better_c_std/vector.h> 

#endif // LIBMAZE_MAZE_STRUCT_H_