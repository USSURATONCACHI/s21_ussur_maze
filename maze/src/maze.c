#include <stdlib.h>
#include <stddef.h>

#include <better_c_std/prettify.h>
#include <libmaze/maze.h>



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


MzMazeResult mz_maze_crop_to_size(const MzMaze* old_one, size_t width, size_t height) {
    MzMazeResult res = mz_maze_create(width, height);

    if (res.is_ok) {
        for (size_t y = 0; y < res.ok.height && y < old_one->height; y++)
            for (size_t x = 0; x < res.ok.width && x < old_one->width; x++)
                mz_maze_set_at(&res.ok, x, y, mz_maze_at(old_one, x, y));
    }

    return res;
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