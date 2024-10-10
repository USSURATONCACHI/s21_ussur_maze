#ifndef LIBMAZE_MAZE_STRUCT_H_
#define LIBMAZE_MAZE_STRUCT_H_

#include <stddef.h>
#include <stdint.h>
#include <better_c_std/result.h>
#include <libmaze/error.h>

// Each cell is 2 bits. Cells are packed row-by-row into a single data buffer (`raw_data`).
// Data buffer is padded for its size to be a multiple of 4 bytes.
//
// Bit index of a cell is calculated as: `(y * width + x) * 2 bits`
typedef struct {
    uint8_t* raw_data;
    size_t width;
    size_t height;
} MzMaze;

typedef STRUCT_RESULT(MzMaze, MzError) MzMazeResult;


#endif // LIBMAZE_MAZE_STRUCT_H_