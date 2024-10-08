#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include <better_c_std/prettify.h>
#include <libmaze/directions_struct.h>
#include <libmaze/cell_struct.h>
#include <libmaze/maze.h>

#define VECTOR_MAKE_STATIC
#define VECTOR_IMPLEMENTATION
#define VECTOR_ITEM_TYPE size_t
#include <better_c_std/vector.h>


static size_t get_buffer_size(size_t w, size_t h) {
    size_t size_bits = w * h * 2; // each cell is 2 bits
    size_t size_bytes = ((size_bits + 31) / 32) * 4; // pad to 4 bytes
    return size_bytes;
}

MzMazeResult mz_maze_create(size_t width, size_t height) {
    size_t size_bytes = get_buffer_size(width, height);
    
    uint8_t* data = size_bytes > 0 ? ((uint8_t*) calloc(size_bytes, 1)) : NULL;
    if (data || size_bytes == 0) {
        MzMaze maze = {
            .width = width,
            .height = height,
            .raw_data = data,
        };
        return (MzMazeResult) OK(maze);
    } else {
        return (MzMazeResult) ERR(NOT_ENOUGH_MEMORY);
    }
}

size_t mz_maze_get_buffer_size(const MzMaze* maze) {
    return get_buffer_size(maze->width, maze->height);
}

void mz_maze_free(MzMaze maze) {
    free(maze.raw_data);
}

MzCell mz_maze_at(const MzMaze* maze, size_t x, size_t y) {
    size_t pos_bits = (y * maze->width + x) * MZ_CELL_SIZE_BITS;
    size_t pos_bytes = pos_bits / 8;
    size_t rem_bits = pos_bits % 8;
    uint8_t bitmask = MZ_CELL_BITMASK(uint8_t, rem_bits);

    uint8_t data_byte = maze->raw_data[pos_bytes];
    uint8_t data = (data_byte & bitmask) >> rem_bits;
    MzCell cell = *((MzCell*)&data);
    return cell;
}
void mz_maze_set_at(MzMaze* maze, size_t x, size_t y, MzCell cell) {
    if (x == 0)
        cell.left_wall = true;
    if (y == 0)
        cell.top_wall = true;

    size_t pos_bits = (y * maze->width + x) * 2;
    size_t pos_bytes = pos_bits / 8;
    size_t rem_bits = pos_bits % 8;
    uint8_t bitmask = MZ_CELL_BITMASK(uint8_t, rem_bits);

    uint8_t data = *((uint8_t*)&cell);
    uint8_t data_byte = (data << rem_bits) & bitmask;
    maze->raw_data[pos_bytes] &= ~bitmask;
    maze->raw_data[pos_bytes] |= data_byte;    
}

void mz_maze_fill_random(MzMaze* maze) {
    debugln("Maze size: %zu %zu", maze->width, maze->height);
    for (size_t y = 0; y < maze->height; y++) {
        for (size_t x = 0; x < maze->width; x++) {
            int r = rand();
            MzCell cell = {
                .top_wall = r & 1 ? true : false,
                .left_wall = r & 2 ? true : false,  
            };

            mz_maze_set_at(maze, x, y, cell);
        }
    }
}


MzDirections mz_maze_where_can_go(const MzMaze* maze, size_t x, size_t y) {
    MzCell cell = mz_maze_at(maze, x, y);

    if (x == 0) cell.left_wall = true;
    if (y == 0) cell.top_wall = true;

    MzDirections result = {
        .up    = !cell.top_wall,
        .left  = !cell.left_wall,
        .right = false,
        .down  = false,
    };

    if (x + 1 < maze->width)
        result.right = !mz_maze_at(maze, x + 1, y).left_wall;

    if (y + 1 < maze->height)
        result.down = !mz_maze_at(maze, x + 1, y).top_wall;
    
    return result;
}

void mz_maze_print(const MzMaze* maze) {
    const char* cells[4][2] = {
        {"+ ", "  "}, // left_wall: false, top_wall: false,
        {"+ ", "| "}, // left_wall: true, top_wall: false,
        {"+-", "  "}, // left_wall: false, top_wall: true,
        {"+-", "| "}, // left_wall: true, top_wall: true,
    };

    for (size_t y = 0; y < maze->height; y++) {
        for (size_t row = 0; row < 2; row++) {
            for (size_t x = 0; x < maze->width; x++) {
                MzCell cell = mz_maze_at(maze, x, y);

                if (x == 0) cell.left_wall = true;
                if (y == 0) cell.top_wall = true;

                int cell_id = (cell.left_wall ? 1 : 0) + (cell.top_wall ? 1 : 0) * 2;
                printf("%s", cells[cell_id][row]);
            }
            if (row % 2 == 0)
                printf("+");
            else
                printf("|");
            printf("\n");
        }
    }

    for (size_t x = 0; x < maze->width; x++)
        printf("+-");
    printf("+\n");
}

static bool can_go_up(MzCell tl, MzCell tr, MzCell bl, MzCell br) {
    if (tl.top_wall && tr.top_wall)
        return false;

    if (bl.top_wall && bl.top_wall)
        return false;

    bool has_wall_on_left = tl.top_wall || bl.top_wall;
    bool has_wall_on_right = tr.top_wall || br.top_wall;
    if (has_wall_on_left && has_wall_on_right && tr.left_wall)
        return false;

    return true;
}

static bool can_go_left(MzCell tl, MzCell tr, MzCell bl, MzCell br) {
    if (tl.left_wall && bl.left_wall)
        return false;

    if (tr.left_wall && br.left_wall)
        return false;

    bool has_wall_on_top = tl.left_wall || tr.left_wall;
    bool has_wall_on_bottom = br.left_wall || bl.left_wall;
    if (has_wall_on_top && has_wall_on_bottom && bl.top_wall)
        return false;

    return true;
}

MzMazeResult mz_maze_get_mipmap_halved(const MzMaze* maze) {
    size_t halved_width = maze->width / 2;
    size_t halved_height = maze->height / 2;

    MzMazeResult halved_res = mz_maze_create(halved_width, halved_height);
    if (!halved_res.is_ok) {
        return (MzMazeResult) ERR(halved_res.error);
    }
    MzMaze halved = halved_res.ok;

    for (size_t y = 0; y < halved_height; y++) {
        for (size_t x = 0; x < halved_width; x++) {
            MzCell tl_cell = mz_maze_at(maze, x * 2 + 0, y * 2 + 0);
            MzCell tr_cell = mz_maze_at(maze, x * 2 + 1, y * 2 + 0);
            MzCell bl_cell = mz_maze_at(maze, x * 2 + 0, y * 2 + 1);
            MzCell br_cell = mz_maze_at(maze, x * 2 + 1, y * 2 + 1); // dont need this cell?

            bool top_wall = !can_go_up(tl_cell, tr_cell, bl_cell, br_cell);
            bool left_wall = !can_go_left(tl_cell, tr_cell, bl_cell, br_cell);
            mz_maze_set_at(&halved, x, y, (MzCell) { 
                .top_wall = top_wall, 
                .left_wall = left_wall
            });
        }
    }

    return (MzMazeResult) OK(halved);
}

// Function to generate a perfect maze using Eller's Algorithm
typedef struct {
    MzMaze maze;
    size_t* row;
    size_t* next_row;
    vec_size_t sets;
    vec_size_t sets_re_buf;
    vec_size_t sets_chosen_cells;
} EllerResources;

typedef STRUCT_RESULT(EllerResources, MzError) EllerResourcesResult;

static EllerResourcesResult eller_allocate_resources(size_t width, size_t height);
static void EllerResources_free(EllerResources res);

static void eller_add_vertical_walls(MzMaze* maze, size_t y, size_t* row, vec_size_t* sets);
static void eller_pick_a_cell_for_each_set(vec_size_t* sets, vec_size_t* sets_chosen_cells);
static void eller_add_horizontal_walls(MzMaze* maze, size_t y, size_t* row, size_t* next_row, vec_size_t* sets, vec_size_t* sets_chosen_cells);
static void eller_remove_empty_sets(size_t width, size_t* row, vec_size_t* sets, vec_size_t* sets_re_buf);

MzMazeResult mz_maze_generate_perfect_eller(size_t width, size_t height) {
    EllerResourcesResult res_result = eller_allocate_resources(width, height);

    if (!res_result.is_ok)
        return (MzMazeResult) ERR(res_result.error);

    if (width == 0 || height == 0)
        return (MzMazeResult) OK(res_result.ok.maze);
    EllerResources res = res_result.ok;

    // Fill with unique sets
    for (size_t i = 0; i < width; i++) {
        res.row[i] = res.sets.length;
        vec_size_t_push(&res.sets, 1); // 1 cell in set
    }

    for (size_t y = height - 1; ; y--) {
        if (y % 64 == 0)
            debugln("y = %zu", y);

        eller_add_vertical_walls(&res.maze, y, res.row, &res.sets);
        eller_pick_a_cell_for_each_set(&res.sets, &res.sets_chosen_cells);
        eller_add_horizontal_walls(&res.maze, y, res.row, res.next_row, &res.sets, &res.sets_chosen_cells);
        SWAP(size_t*, res.row, res.next_row);
        eller_remove_empty_sets(width, res.row, &res.sets, &res.sets_re_buf);

        if (y == 0) break;
    }

    MzMaze maze = res.maze;
    res.maze = (MzMaze){ .raw_data = NULL };

    debugln("Freeing...");
    EllerResources_free(res);
    debugln("Freed");
    return (MzMazeResult) OK(maze);
}

static EllerResourcesResult eller_allocate_resources(size_t width, size_t height) {
    MzMazeResult maze_res = mz_maze_create(width, height);
    if (!maze_res.is_ok)
        return (EllerResourcesResult) ERR(maze_res.error);

    EllerResources res = {
        .maze              = maze_res.ok,
        .row               = NULL,
        .next_row          = NULL,
        .sets              = { NULL },
        .sets_re_buf       = { NULL },
        .sets_chosen_cells = { NULL },
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
    mz_maze_free(res.maze);
    vec_size_t_free(res.sets_chosen_cells);
    vec_size_t_free(res.sets);
    vec_size_t_free(res.sets_re_buf);
    free(res.row);
    free(res.next_row);
}

static void eller_add_vertical_walls(MzMaze* maze, size_t y, size_t* row, vec_size_t* sets) {
    size_t width = maze->width;

    for (size_t x = width - 1; x >= 1; x--) {
        
        bool should_add_left_wall = (rand() & 1) > 0;
        if (y == 0)
            should_add_left_wall = false;

        if (row[x] == row[x - 1] || should_add_left_wall) {
            MzCell cell = mz_maze_at(maze, x, y);
            cell.left_wall = true;
            mz_maze_set_at(maze, x, y, cell);
        } else {
            // merge sets
            size_t replace_from = row[x - 1];
            size_t replace_from_size = sets->data[replace_from];
            sets->data[replace_from] = 0;
            size_t replace_to = row[x];

            for (size_t x1 = 0; replace_from_size != 0 && x1 < width; x1++) {
                if (row[x1] == replace_from) {
                    row[x1] = replace_to;
                    sets->data[replace_to]++;
                    replace_from_size--;
                }
            }
        }

        if (x == 0) break;
    }
}

static void eller_pick_a_cell_for_each_set(vec_size_t* sets, vec_size_t* sets_chosen_cells) {
    sets_chosen_cells->length = 0; // clear vector
    for (size_t i = 0; i < sets->length; i++) {
        size_t set_size = sets->data[i];
        size_t chosen_cell = 0;
        if (set_size > 1)
            chosen_cell = ((size_t)rand()) % set_size;
        vec_size_t_push(sets_chosen_cells, chosen_cell);
    }
}

static void eller_add_horizontal_walls(MzMaze* maze, size_t y, size_t* row, size_t* next_row, vec_size_t* sets, vec_size_t* sets_chosen_cells) {
    size_t width = maze->width;
    size_t prev_set_id = SIZE_MAX;
    size_t cell_in_set_id = 0;
    memset(sets->data, 0, sizeof(size_t) * sets->length); // fill with zeros
    for (size_t x = width - 1; true; x--) {
        size_t set_id = row[x];
        if (prev_set_id != set_id) {
            cell_in_set_id = 0;
            prev_set_id = set_id;
        } else {
            cell_in_set_id++;
        }

        size_t set_size = sets->data[set_id];
        size_t chosen_cell = sets_chosen_cells->data[set_id];
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

            next_row[x] = sets->length;
            vec_size_t_push(sets, 1);
        } else {
            next_row[x] = row[x];
            sets->data[next_row[x]]++;
        }
        if (x == 0) break;
    }
}

static void eller_remove_empty_sets(size_t width, size_t* row, vec_size_t* sets, vec_size_t* sets_re_buf) {
    sets_re_buf->length = 0;
    size_t sets_count = sets->length;
    for (size_t set_id = 0; set_id < sets_count; set_id++) {
        if (sets->data[set_id] > 0) { // not empty set
            size_t new_set_id = sets_re_buf->length;
            vec_size_t_push(sets_re_buf, sets->data[set_id]); // move set to a new vector
            sets->data[set_id] = new_set_id; // temporarily write set id into the old set location
        }
    }

    for (size_t x = 0; x < width; x++)
        row[x] = sets->data[row[x]]; // update sets ids

    SWAP(vec_size_t, *sets_re_buf, *sets);
}