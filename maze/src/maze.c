#include <stdint.h>
#include <stdlib.h>

#include <better_c_std/prettify.h>
#include <libmaze/directions_struct.h>
#include <libmaze/cell_struct.h>
#include <libmaze/maze.h>

MzMaze mz_maze_create(size_t width, size_t height) {
    size_t size_bits = width * height * 2; // each cell is 2 bits
    size_t size_bytes = (size_bits + 7) / 8;
    
    uint8_t* data = (uint8_t*) calloc(size_bytes, 1);
    assert_alloc(data);

    return (MzMaze) {
        .width = width,
        .height = height,
        .raw_data = data,
    };
}

size_t mz_maze_get_buffer_size(const MzMaze* maze) {
    size_t size_bits = maze->width * maze->height * 2; // each cell is 2 bits
    size_t size_bytes = (size_bits + 7) / 8;
    return size_bytes;
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