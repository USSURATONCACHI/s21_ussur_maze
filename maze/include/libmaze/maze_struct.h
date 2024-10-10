#ifndef LIBMAZE_MAZE_STRUCT_H_
#define LIBMAZE_MAZE_STRUCT_H_

#include <stddef.h>
#include <stdint.h>
#include <better_c_std/result.h>
#include <libmaze/error.h>

typedef struct {
    uint8_t* raw_data;
    size_t width;
    size_t height;
} MzMaze;

typedef STRUCT_RESULT(MzMaze, MzError) MzMazeResult;


#endif // LIBMAZE_MAZE_STRUCT_H_