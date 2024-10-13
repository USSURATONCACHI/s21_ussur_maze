#include <stdlib.h>
#include <stddef.h>

#include <better_c_std/prettify.h>
#include <libmaze/maze.h>



static size_t get_buffer_size(size_t w, size_t h) {
    size_t size_bits = w * h * 2; // each cell is 2 bits
    size_t size_bytes = ((size_bits + 31) / 32) * 4; // pad to 4 bytes
    return size_bytes;
}

MzMazeResult MzMaze_create(size_t width, size_t height) {
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
        return (MzMazeResult) ERR(MZ_ERROR_NOT_ENOUGH_MEMORY);
    }
}

size_t MzMaze_get_buffer_size(const MzMaze* maze) {
    return get_buffer_size(maze->width, maze->height);
}

void MzMaze_free(MzMaze maze) {
    free(maze.raw_data);
}

MzCell MzMaze_at(const MzMaze* maze, size_t x, size_t y) {
    size_t pos_bits = (y * maze->width + x) * MZ_CELL_SIZE_BITS;
    size_t pos_bytes = pos_bits / 8;
    size_t rem_bits = pos_bits % 8;
    uint8_t bitmask = MZ_CELL_BITMASK(uint8_t, rem_bits);

    uint8_t data_byte = maze->raw_data[pos_bytes];
    uint8_t data = (data_byte & bitmask) >> rem_bits;
    MzCell cell = *((MzCell*)&data);
    return cell;
}
void MzMaze_set_at(MzMaze* maze, size_t x, size_t y, MzCell cell) {
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



MzDirections MzMaze_where_can_go(const MzMaze* maze, size_t x, size_t y) {
    MzCell cell = MzMaze_at(maze, x, y);

    if (x == 0) cell.left_wall = true;
    if (y == 0) cell.top_wall = true;

    MzDirections result = {
        .up    = !cell.top_wall,
        .left  = !cell.left_wall,
        .right = false,
        .down  = false,
    };

    if (x + 1 < maze->width)
        result.right = !MzMaze_at(maze, x + 1, y).left_wall;

    if (y + 1 < maze->height)
        result.down = !MzMaze_at(maze, x + 1, y).top_wall;
    
    return result;
}


MzMazeResult MzMaze_crop_to_size(const MzMaze* old_one, size_t width, size_t height) {
    MzMazeResult res = MzMaze_create(width, height);

    if (res.is_ok) {
        for (size_t y = 0; y < res.ok.height && y < old_one->height; y++)
            for (size_t x = 0; x < res.ok.width && x < old_one->width; x++)
                MzMaze_set_at(&res.ok, x, y, MzMaze_at(old_one, x, y));
    }

    return res;
}

void MzMaze_fill_random(MzMaze* maze) {
    debugln("Maze size: %zu %zu", maze->width, maze->height);
    for (size_t y = 0; y < maze->height; y++) {
        for (size_t x = 0; x < maze->width; x++) {
            int r = rand();
            MzCell cell = {
                .top_wall = r & 1 ? true : false,
                .left_wall = r & 2 ? true : false,  
            };

            MzMaze_set_at(maze, x, y, cell);
        }
    }
}