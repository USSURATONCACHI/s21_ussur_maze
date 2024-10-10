#include <libmaze/maze.h>
#include <libmaze/vec_size_t.h>
#include <better_c_std/prettify.h>


static void add_vertical_walls      (MzEllersResources* res, MzMaze* maze, size_t y);
static void pick_a_cell_for_each_set(MzEllersResources* res);
static void add_horizontal_walls    (MzEllersResources* res, MzMaze* maze, size_t y);
static void remove_empty_sets       (MzEllersResources* res, size_t width);


void MzMaze_fill_perfect_eller_preallocated(MzMaze* maze, MzEllersResources* res, size_t* current_row_report) {
    if (maze == NULL || maze->width == 0 || maze->height == 0)
        return;

    // Memset maze to have no walls
    memset(maze->raw_data, 0, MzMaze_get_buffer_size(maze));

    // Initialize with unique sets
    size_t width = maze->width;
    size_t height = maze->height;
    for (size_t i = 0; i < width; i++) {
        res->row[i] = res->sets_sizes.length;
        vec_size_t_push(&res->sets_sizes, 1); // 1 cell in this set
    }

    // Reset progress value
    if (current_row_report)
        *current_row_report = 0;

    // Run for each row
    for (size_t y = height - 1; ; y--) {
        if (y % 64 == 0)
            debugln("y = %zu", y);

        add_vertical_walls(res, maze, y);
        pick_a_cell_for_each_set(res);
        add_horizontal_walls(res, maze, y);
        SWAP(size_t*, res->row, res->next_row);
        remove_empty_sets(res, width);

        if (current_row_report)
            (*current_row_report)++;

        if (y == 0) break; // `(size_t)x >= 0` is always true
    }
}

MzVoidResult MzMaze_fill_perfect_eller(MzMaze* maze) {
    if (maze == NULL || maze->width == 0 || maze->height == 0)
        return (MzVoidResult) OK({});

    // Pre-allocate everything (often allocations would ruin perfomance)
    MzEllersResourcesResult res_result = MzEllersResources_allocate(maze->width);
    if (!res_result.is_ok)
        return (MzVoidResult) ERR(res_result.error);
    MzEllersResources res = res_result.ok;

    MzMaze_fill_perfect_eller_preallocated(maze, &res, NULL); // run
    MzEllersResources_free(res);

    return (MzVoidResult) OK({});
}

MzMazeResult MzMaze_create_perfect_eller(size_t width, size_t height) {
    MzMazeResult empty_maze = MzMaze_create(width, height);
    if (!empty_maze.is_ok)
        return (MzMazeResult) ERR(empty_maze.error);

    MzVoidResult fill_result = MzMaze_fill_perfect_eller(&empty_maze.ok);
    if (!fill_result.is_ok) {
        MzMaze_free(empty_maze.ok);
        return (MzMazeResult) ERR(fill_result.error);
    }

    return (MzMazeResult) OK(empty_maze.ok); // `empty_maze` is not empty anymore
}

// Static functions

static void add_vertical_walls(MzEllersResources* res, MzMaze* maze, size_t y) {
    size_t width = maze->width;

    for (size_t x = width - 1; x >= 1; x--) {
        
        bool should_add_left_wall = (rand() & 1) > 0;
        if (y == 0)
            should_add_left_wall = false;

        if (res->row[x] == res->row[x - 1] || should_add_left_wall) {
            MzCell cell = MzMaze_at(maze, x, y);
            cell.left_wall = true;
            MzMaze_set_at(maze, x, y, cell);
        } else {
            // merge sets
            size_t replace_from = res->row[x - 1];
            size_t replace_to = res->row[x];

            // All cells (size) of one set is transferred to another
            size_t replace_from_size = res->sets_sizes.data[replace_from];
            res->sets_sizes.data[replace_from] = 0;
            res->sets_sizes.data[replace_to] += replace_from_size;

            // This loop is the main perfomance bottleneck. Havent yet come up with a solution to this
            for (size_t x1 = 0; replace_from_size != 0 && x1 < width; x1++) {
                if (res->row[x1] == replace_from) {
                    res->row[x1] = replace_to;
                    replace_from_size--;
                }
            }
        }

        if (x == 0) break;
    }
}

static void pick_a_cell_for_each_set(MzEllersResources* res) {
    res->sets_chosen_cells.length = 0; // clear vector, but preserve capacity

    for (size_t i = 0; i < res->sets_sizes.length; i++) {
        size_t set_size = res->sets_sizes.data[i];

        size_t chosen_cell = 0;
        if (set_size > 1)
            chosen_cell = ((size_t)rand()) % set_size;

        vec_size_t_push(&res->sets_chosen_cells, chosen_cell);
    }
}

static void add_horizontal_walls(MzEllersResources* res, MzMaze* maze, size_t y) {
    // Keep track of ID of cell in it's respective set
    size_t prev_set_id = SIZE_MAX;
    size_t cell_in_set_id = 0;
    
    // We also recalculate sizes of sets in this loop
    assert_m(res->sets_re_buf.capacity >= res->sets_sizes.length);
    res->sets_re_buf.length = res->sets_sizes.length;
    memset(res->sets_re_buf.data, 0, sizeof(size_t) * res->sets_re_buf.length);

    // Run for each vell
    for (size_t x = maze->width - 1; ; x--) {
        // Keep track of ID of cell in it's respective set
        size_t set_id = res->row[x];
        if (prev_set_id != set_id) {
            cell_in_set_id = 0;
            prev_set_id = set_id;
        } else {
            cell_in_set_id++;
        }
        
        size_t set_size = res->sets_sizes.data[set_id];
        size_t chosen_cell = res->sets_chosen_cells.data[set_id];

        // Decide if should put a horizontal wall
        bool put_horz_wall;

        if (cell_in_set_id == chosen_cell)
            put_horz_wall = false;          // Chosen cell cannot have a horizontal wall 
        else if (cell_in_set_id == (set_size - 1 - chosen_cell))
            put_horz_wall = true;           // This is to not mess up probability biases
        else
            put_horz_wall = (rand() & 1) > 0;
        
        // Put wall or preserve set
        if (put_horz_wall) {
            MzCell cell = MzMaze_at(maze, x, y);
            cell.top_wall = true;
            MzMaze_set_at(maze, x, y, cell);

            // Init this cell with a new set
            res->next_row[x] = res->sets_re_buf.length;
            vec_size_t_push(&res->sets_re_buf, 1);
        } else {
            // Keep same set
            size_t set_id = res->row[x];
            res->next_row[x] = set_id;
            res->sets_re_buf.data[set_id]++;
        }
        if (x == 0) break;
    }

    // Sizes recalculated
    SWAP(vec_size_t, res->sets_re_buf, res->sets_sizes);
}

static void remove_empty_sets(MzEllersResources* res, size_t width) {
    res->sets_re_buf.length = 0; // clean, but preserve capacity

    size_t sets_count = res->sets_sizes.length;
    for (size_t set_id = 0; set_id < sets_count; set_id++) {
        // forget empty set
        if (res->sets_sizes.data[set_id] == 0)
            continue; 

        // keep non-empty set
        size_t new_set_id = res->sets_re_buf.length;
        vec_size_t_push(&res->sets_re_buf, res->sets_sizes.data[set_id]);

        res->sets_sizes.data[set_id] = new_set_id; // temporarily write updated set id into its size (a bit hacky)
    }

    // Update sets IDs in a row
    for (size_t x = 0; x < width; x++) {
        size_t set_id = res->row[x];
        size_t updated_set_id = res->sets_sizes.data[set_id]; // temporarily not a size
        res->row[x] = updated_set_id;
    }

    SWAP(vec_size_t, res->sets_re_buf, res->sets_sizes);
}