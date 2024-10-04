#ifndef LIBMAZE_CELL_STRUCT_H_
#define LIBMAZE_CELL_STRUCT_H_

#include <stdbool.h>

#define MZ_CELL_SIZE_BITS 2
#define MZ_CELL_BITMASK(type, pos) (((type) 3) << pos)
typedef struct {
    bool top_wall : 1;
    bool left_wall : 1;
} MzCell;

#endif // LIBMAZE_CELL_STRUCT_H_