#ifndef LIBMAZE_ELLERS_RESOURCES_H_
#define LIBMAZE_ELLERS_RESOURCES_H_

#include <stddef.h>
#include <stdbool.h>
#include <libmaze/vec_size_t.h>
#include <libmaze/error.h>
#include <better_c_std/result.h>

// Resources needed for running Eller's algorithm row by row.
typedef struct {
    size_t* row;      // Sets IDs for each cell in a row
    size_t* next_row; // Temporary write swapbuffer

    // Sets data is not aggregated into a single struct for perfomance.
    // These vectors are cleaned up and refilled at different times.
    // When these are split, you can just memset their buffers or reset lengths to 0. It would not be possible, if these were a single struct

    vec_size_t sets_sizes;          // Amount of cells in sets 
    vec_size_t sets_chosen_cells;   // Ids of cells in the set, that are forbidden from having a horizontal wall
    vec_size_t sets_re_buf;         // Preallocated swapbuffer to move non-empty sets into it
} MzEllersResources;

typedef STRUCT_RESULT(MzEllersResources, MzError) MzEllersResourcesResult; 

MzEllersResourcesResult MzEllersResources_allocate(size_t row_width);
void MzEllersResources_free(MzEllersResources res);

#endif // LIBMAZE_ELLERS_RESOURCES_H_