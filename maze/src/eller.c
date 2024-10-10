#include <libmaze/maze.h>
#include <better_c_std/prettify.h>

#define VECTOR_MAKE_STATIC
#define VECTOR_IMPLEMENTATION
#define VECTOR_ITEM_TYPE size_t
#include <better_c_std/vector.h>

// Function to generate a perfect maze using Eller's Algorithm
typedef struct {
    size_t* row;      // Sets IDs for each cell in a row
    size_t* next_row; // Temporary write buffer

    vec_size_t sets;                // Each entry is a set (with set id = index of entry). Value of the entry is the amout of cells in it (in the current row) 
    vec_size_t sets_re_buf;         // Temporary buffer to move sets into, for cleanup of empty sets
    vec_size_t sets_chosen_cells;   // Ids of 
} EllerResources;

typedef STRUCT_RESULT(EllerResources, MzError) EllerResourcesResult;

static EllerResourcesResult eller_allocate_resources(size_t width, size_t height);
static void EllerResources_free(EllerResources res);

static void eller_add_vertical_walls      (MzMaze* maze, size_t y, EllerResources* res);
static void eller_pick_a_cell_for_each_set(EllerResources* res);
static void eller_add_horizontal_walls    (MzMaze* maze, size_t y, EllerResources* res);
static void eller_remove_empty_sets       (size_t width, EllerResources* res);

MzVoidResult mz_maze_fill_perfect_eller(MzMaze* maze) {
    if (maze == NULL || maze->width == 0 || maze->height == 0)
        return (MzVoidResult) OK({});

    // Pre-allocate buffers for that operations (often allocations would ruin perfomance of this)
    EllerResourcesResult res_result = eller_allocate_resources(maze->width, maze->height);
    if (!res_result.is_ok)
        return (MzVoidResult) ERR(res_result.error);
    EllerResources res = res_result.ok;

    // Initialize with unique sets
    size_t width = maze->width;
    size_t height = maze->height;
    for (size_t i = 0; i < width; i++) {
        res.row[i] = res.sets.length;
        vec_size_t_push(&res.sets, 1); // 1 cell in set
    }

    for (size_t y = height - 1; ; y--) {
        if (y % 64 == 0)
            debugln("y = %zu", y);

        eller_add_vertical_walls(maze, y, &res);
        eller_pick_a_cell_for_each_set(&res);
        eller_add_horizontal_walls(maze, y, &res);
        SWAP(size_t*, res.row, res.next_row);
        eller_remove_empty_sets(width, &res);

        if (y == 0) break;
    }

    debugln("Freeing...");
    EllerResources_free(res);
    debugln("Freed");
    return (MzVoidResult) OK({});
}

MzMazeResult mz_maze_generate_perfect_eller(size_t width, size_t height) {
    MzMazeResult empty_maze = mz_maze_create(width, height);
    if (!empty_maze.is_ok)
        return (MzMazeResult) ERR(empty_maze.error);

    MzVoidResult fill_result = mz_maze_fill_perfect_eller(&empty_maze.ok);
    if (!fill_result.is_ok) {
        mz_maze_free(empty_maze.ok);
        return (MzMazeResult) ERR(fill_result.error);
    }

    return (MzMazeResult) OK(empty_maze.ok); // it is not empty anymore
}

static EllerResourcesResult eller_allocate_resources(size_t width, size_t height) {
    EllerResources res = {
        .row               = NULL,
        .next_row          = NULL,
        .sets              = { NULL, 0, 0 },
        .sets_re_buf       = { NULL, 0, 0 },
        .sets_chosen_cells = { NULL, 0, 0 },
    };
    if (width == 0 || height == 0)
        return (EllerResourcesResult) OK(res);

    // Create first row
    res.row      = (size_t*) malloc(width * sizeof(size_t));
    res.next_row = (size_t*) malloc(width * sizeof(size_t));

    // Keep track of sets
    res.sets              = vec_size_t_with_capacity_try(width);
    res.sets_re_buf       = vec_size_t_with_capacity_try(width);
    res.sets_chosen_cells = vec_size_t_with_capacity_try(width);

    if (
        res.row == NULL                     ||
        res.next_row == NULL                ||
        res.sets.capacity == 0              ||
        res.sets_re_buf.capacity == 0       ||
        res.sets_chosen_cells.capacity == 0 ||
        false
    ) {
        EllerResources_free(res);
        return (EllerResourcesResult) ERR(NOT_ENOUGH_MEMORY);
    }

    return (EllerResourcesResult) OK(res);
}

static void EllerResources_free(EllerResources res) {
    vec_size_t_free(res.sets_chosen_cells);
    vec_size_t_free(res.sets);
    vec_size_t_free(res.sets_re_buf);
    free(res.row);
    free(res.next_row);
}

static void eller_add_vertical_walls(MzMaze* maze, size_t y, EllerResources* res) {
    size_t width = maze->width;

    for (size_t x = width - 1; x >= 1; x--) {
        
        bool should_add_left_wall = (rand() & 1) > 0;
        if (y == 0)
            should_add_left_wall = false;

        if (res->row[x] == res->row[x - 1] || should_add_left_wall) {
            MzCell cell = mz_maze_at(maze, x, y);
            cell.left_wall = true;
            mz_maze_set_at(maze, x, y, cell);
        } else {
            // merge sets
            size_t replace_from = res->row[x - 1];
            size_t replace_from_size = res->sets.data[replace_from];
            res->sets.data[replace_from] = 0;
            size_t replace_to = res->row[x];

            for (size_t x1 = 0; replace_from_size != 0 && x1 < width; x1++) {
                if (res->row[x1] == replace_from) {
                    res->row[x1] = replace_to;
                    res->sets.data[replace_to]++;
                    replace_from_size--;
                }
            }
        }

        if (x == 0) break;
    }
}

static void eller_pick_a_cell_for_each_set(EllerResources* res) {
    res->sets_chosen_cells.length = 0; // clear vector without deallocations
    for (size_t i = 0; i < res->sets.length; i++) {
        size_t set_size = res->sets.data[i];
        size_t chosen_cell = 0;
        if (set_size > 1)
            chosen_cell = ((size_t)rand()) % set_size;
        vec_size_t_push(&res->sets_chosen_cells, chosen_cell);
    }
}

static void eller_add_horizontal_walls(MzMaze* maze, size_t y, EllerResources* res) {
    size_t width = maze->width;
    size_t prev_set_id = SIZE_MAX;
    size_t cell_in_set_id = 0;
    memset(res->sets.data, 0, sizeof(size_t) * res->sets.length); // fill with zeros
    for (size_t x = width - 1; true; x--) {
        size_t set_id = res->row[x];
        if (prev_set_id != set_id) {
            cell_in_set_id = 0;
            prev_set_id = set_id;
        } else {
            cell_in_set_id++;
        }

        size_t set_size = res->sets.data[set_id];
        size_t chosen_cell = res->sets_chosen_cells.data[set_id];
        bool put_horz_wall = true;
        if (cell_in_set_id == chosen_cell) {
            put_horz_wall = false;
        } else if (cell_in_set_id == (set_size - 1 - chosen_cell)) {
            put_horz_wall = true;
        } else {
            put_horz_wall = (rand() & 1) > 0;
        }

        if (put_horz_wall) {
            MzCell cell = mz_maze_at(maze, x, y);
            cell.top_wall = true;
            mz_maze_set_at(maze, x, y, cell);

            res->next_row[x] = res->sets.length;
            vec_size_t_push(&res->sets, 1);
        } else {
            res->next_row[x] = res->row[x];
            res->sets.data[res->next_row[x]]++;
        }
        if (x == 0) break;
    }
}

static void eller_remove_empty_sets(size_t width, EllerResources* res) {
    res->sets_re_buf.length = 0;
    size_t sets_count = res->sets.length;
    for (size_t set_id = 0; set_id < sets_count; set_id++) {
        if (res->sets.data[set_id] > 0) { // not empty set
            size_t new_set_id = res->sets_re_buf.length;
            vec_size_t_push(&res->sets_re_buf, res->sets.data[set_id]); // move set to a new vector
            res->sets.data[set_id] = new_set_id; // temporarily write set id into the old set location
        }
    }

    for (size_t x = 0; x < width; x++)
        res->row[x] = res->sets.data[res->row[x]]; // update sets ids

    SWAP(vec_size_t, res->sets_re_buf, res->sets);
}