#include <stdlib.h>
#include <libmaze/ellers_resources.h>

MzEllersResourcesResult MzEllersResources_allocate(size_t row_width) {
    MzEllersResources res = {
        .row               = NULL,
        .next_row          = NULL,
        .sets_re_buf       = { NULL, 0, 0 },
        .sets_chosen_cells = { NULL, 0, 0 },
        .sets_sizes        = { NULL, 0, 0 },
    };
    if (row_width == 0)
        return (MzEllersResourcesResult) OK(res);

    // Create first row
    res.row      = (size_t*) malloc(row_width * sizeof(size_t));
    res.next_row = (size_t*) malloc(row_width * sizeof(size_t));

    // Keep track of sets
    res.sets_sizes        = vec_size_t_with_capacity_try(row_width);
    res.sets_re_buf       = vec_size_t_with_capacity_try(row_width);
    res.sets_chosen_cells = vec_size_t_with_capacity_try(row_width);

    if (
        res.row == NULL                     ||
        res.next_row == NULL                ||
        res.sets_sizes.capacity == 0        ||
        res.sets_re_buf.capacity == 0       ||
        res.sets_chosen_cells.capacity == 0 ||
        false
    ) {
        // If any single one failed to preallocate - fail
        MzEllersResources_free(res);
        return (MzEllersResourcesResult) ERR(MZ_ERROR_NOT_ENOUGH_MEMORY);
    }

    return (MzEllersResourcesResult) OK(res);
}

void MzEllersResources_free(MzEllersResources res) {
    vec_size_t_free(res.sets_chosen_cells);
    vec_size_t_free(res.sets_sizes);
    vec_size_t_free(res.sets_re_buf);
    free(res.row);
    free(res.next_row);
}