#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include <better_c_std/prettify.h>
#include <libmaze/directions_struct.h>
#include <libmaze/cell_struct.h>
#include <libmaze/maze.h>

#undef VECTOR_H
#undef VECTOR_C
#undef VECTOR_NO_HEADERS
#define VECTOR_MAKE_STATIC
#define VECTOR_IMPLEMENTATION
#define VECTOR_ITEM_TYPE size_t
#include <better_c_std/vector.h>
#undef VECTOR_MAKE_STATIC
#undef VECTOR_IMPLEMENTATION
#undef VECTOR_ITEM_TYPE


static size_t get_buffer_size(size_t w, size_t h) {
    size_t size_bits = w * h * 2; // each cell is 2 bits
    size_t size_bytes = ((size_bits + 31) / 32) * 4; // pad to 4 bytes
    return size_bytes;
}

MzMaze mz_maze_create(size_t width, size_t height) {
    size_t size_bytes = get_buffer_size(width, height);
    
    uint8_t* data = (uint8_t*) calloc(size_bytes, 1);
    assert_alloc(data);

    return (MzMaze) {
        .width = width,
        .height = height,
        .raw_data = data,
    };
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
    MzCell bottom_cell = mz_maze_at(maze, x, y + 1);

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

MzMaze mz_maze_get_mipmap_halved(const MzMaze* maze) {
    size_t halved_width = maze->width / 2;
    size_t halved_height = maze->height / 2;

    MzMaze halved = mz_maze_create(halved_width, halved_height);

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

    return halved;
}

vec_MzMaze mz_maze_generate_mipmaps(const MzMaze* maze) {
    vec_MzMaze mipmaps = vec_MzMaze_create();

    const MzMaze* last_mipmap = maze;
    while (last_mipmap->width > 1 && last_mipmap->height > 1) {
        MzMaze halved = mz_maze_get_mipmap_halved(last_mipmap);
        vec_MzMaze_push(&mipmaps, halved);
        last_mipmap = &mipmaps.data[mipmaps.length - 1];
    }

    return mipmaps;
}


// Function to generate a perfect maze using Eller's Algorithm
void mz_maze_generate_perfect_eller(MzMaze* maze) {
    assert_m(maze != NULL);
    assert_m(maze->width > 0 && maze->height > 0);

    size_t width = maze->width;
    size_t height = maze->height;

    // Create first row
    size_t* row = calloc(width, sizeof(size_t));
    size_t* next_row = calloc(width, sizeof(size_t));
    assert_alloc(row);
    assert_alloc(next_row);

    // Keep track of sets
    vec_size_t sets = vec_size_t_with_capacity(width * 2);
    vec_size_t sets_re_buf = vec_size_t_with_capacity(width * 2);
    vec_size_t sets_chosen_cells = vec_size_t_with_capacity(width * 2);

    // Fill with unique sets
    for (size_t i = 0; i < width; i++) {
        row[i] = sets.length;
        vec_size_t_push(&sets, 1); // 1 cell in set
    }

    

    for (size_t y = height - 1; ; y--) {
        if (y % 100 == 0)
            debugln("y = %zu", y);
        // 1. Add vertical walls
        for (size_t x = width - 1; x >= 1; x--) {
            MzCell cell = mz_maze_at(maze, x, y);

            bool should_add_left_wall = ((rand() % 100) + 1) < 50;
            if (should_add_left_wall) {
                cell.left_wall = true;
            } else {
                sets.data[row[x - 1]]--; // -1 from set size
                sets.data[row[x]]++;     // +1 to current set
                row[x - 1] = row[x];
            }

            mz_maze_set_at(maze, x, y, cell);

            if (x == 0) break;
        }

        // 2. Pick a cell for each set
        sets_chosen_cells.length = 0; // clear vector
        for (size_t i = 0; i < sets.length; i++) {
            size_t set_size = sets.data[i];
            size_t chosen_cell = 0;
            if (set_size > 1)
                chosen_cell = ((size_t)rand()) % set_size;
            vec_size_t_push(&sets_chosen_cells, chosen_cell);
        }

        // 2. Add horizontal walls
        size_t prev_set_id = SIZE_MAX;
        size_t cell_in_set_id = 0;
        for (size_t x = width - 1; x >= 0; x--) {
            size_t set_id = row[x];
            if (prev_set_id != set_id) {
                cell_in_set_id = 0;
                prev_set_id = set_id;
            } else {
                cell_in_set_id++;
            }

            if (cell_in_set_id != sets_chosen_cells.data[set_id]) {
                MzCell cell = mz_maze_at(maze, x, y);
                cell.top_wall = true;
                mz_maze_set_at(maze, x, y, cell);
            }
            if (x == 0) break;
        }

        // 3. Determine if next row should have same set & update sets sizes
        memset(sets.data, 0, sizeof(size_t) * sets.length); // fill with zeros
        for (size_t x = width - 1; x >= 0; x--) {
            next_row[x] = row[x];

            MzCell cell = mz_maze_at(maze, x, y);
            if (cell.top_wall) { // assign a new set to it
                next_row[x] = sets.length;
                vec_size_t_push(&sets, 0);
            }

            sets.data[next_row[x]]++;
            if (x == 0) break;
        }

        SWAP(size_t*, row, next_row);

        // 4. Filter out all non-empty sets
        sets_re_buf.length = 0;
        for (size_t set_id = 0; set_id < sets.length; set_id++) {
            if (sets.data[set_id] > 0) { // not empty set
                size_t new_set_id = sets_re_buf.length;
                vec_size_t_push(&sets_re_buf, sets.data[set_id]); // move set to a new vector
                sets.data[set_id] = new_set_id; // temporarily write set id into the old set location
            }
        }
        for (size_t x = 0; x < width; x++)
            row[x] = sets.data[row[x]]; // update sets ids
        SWAP(vec_size_t, sets_re_buf, sets);

        if (y == 0) break;
    }

    vec_size_t_free(sets_chosen_cells);
    vec_size_t_free(sets);
    vec_size_t_free(sets_re_buf);
    free(row);
    free(next_row);
}


#define VECTOR_C MzMaze
#define VECTOR_ITEM_DESTRUCTOR mz_maze_free
#include <better_c_std/vector.h> 