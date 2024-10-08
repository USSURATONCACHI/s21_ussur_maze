#ifndef LIBMAZE_MAZE_STRUCT_H_
#define LIBMAZE_MAZE_STRUCT_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t* raw_data;
    size_t width;
    size_t height;
} MzMaze;

#include <better_c_std/result.h>

typedef enum {
    NOT_ENOUGH_MEMORY,
} MzError;

typedef STRUCT_RESULT(MzMaze, MzError) MzMazeResult;

#endif // LIBMAZE_MAZE_STRUCT_H_