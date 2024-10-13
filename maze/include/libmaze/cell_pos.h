#ifndef LIBMAZE_CELL_POS_H_
#define LIBMAZE_CELL_POS_H_

#include <stddef.h>

typedef struct {
    size_t x, y;
} MzCellPos;

#define VECTOR_ITEM_TYPE MzCellPos
#include <better_c_std/vector.h>

#endif // LIBMAZE_CELL_POS_H_