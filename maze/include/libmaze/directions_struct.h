#ifndef LIBMAZE_DIRECTIONS_STRUCT_H_
#define LIBMAZE_DIRECTIONS_STRUCT_H_

#include <stdbool.h>

typedef struct {
    bool up : 1;
    bool down : 1;
    bool left : 1;
    bool right : 1;
} MzDirections;

#endif // LIBMAZE_DIRECTIONS_STRUCT_H_