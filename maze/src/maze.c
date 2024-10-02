#include <stdint.h>
#include <stdlib.h>

#include <libmaze/directions_struct.h>
#include <libmaze/cell_struct.h>
#include <libmaze/maze.h>
#include <libmaze/util.h>

Maze maze_create(size_t width, size_t height) {
    size_t size_bits = width * height * 2; // each cell is 2 bits
    size_t size_bytes = (size_bits + 7) / 8;
    
    uint8_t* data = (uint8_t*) calloc(size_bytes, 1);
    assert_alloc(data);

    return (Maze) {
        .width = width,
        .height = height,
        .raw_data = data,
    };
}
void maze_free(Maze maze) {
    free(maze.raw_data);
}

MazeCell maze_at(Maze maze, size_t x, size_t y) {
    size_t pos_bits = (y * maze.width + x) * MAZE_CELL_SIZE_BITS;
    size_t pos_bytes = pos_bits / 8;
    size_t rem_bits = pos_bits % 8;
    uint8_t bitmask = MAZE_CELL_BITMASK(uint8_t, rem_bits);

    uint8_t data_byte = maze.raw_data[pos_bytes];
    uint8_t data = (data_byte & bitmask) >> rem_bits;
    MazeCell cell = *((MazeCell*)&data);
    return cell;
}
void maze_set_at(Maze maze, size_t x, size_t y, MazeCell cell) {
    size_t pos_bits = (y * maze.width + x) * 2;
    size_t pos_bytes = pos_bits / 8;
    size_t rem_bits = pos_bits % 8;
    uint8_t bitmask = MAZE_CELL_BITMASK(uint8_t, rem_bits);

    uint8_t data = *((uint8_t*)&cell);
    uint8_t data_byte = (data << rem_bits) & bitmask;
    maze.raw_data[pos_bytes] &= ~bitmask;
    maze.raw_data[pos_bytes] |= data_byte;    
}

void maze_fill_random(Maze maze) {
    for (size_t y = 0; y < maze.height; y++) {
        for (size_t x = 0; x < maze.width; x++) {
            int r = rand();
            MazeCell cell = {
                .top_wall = r & 1 ? true : false,
                .left_wall = r & 2 ? true : false,  
            };

            maze_set_at(maze, x, y, cell);
        }
    }
}


MazeDirections maze_where_can_go(Maze maze, size_t x, size_t y) {
    MazeCell cell = maze_at(maze, x, y);
    MazeCell bottom_cell = maze_at(maze, x, y + 1);

    if (x == 0) cell.left_wall = true;
    if (y == 0) cell.top_wall = true;

    MazeDirections result = {
        .up    = !cell.top_wall,
        .left  = !cell.left_wall,
        .right = false,
        .down  = false,
    };

    if (x + 1 < maze.width)
        result.right = !maze_at(maze, x + 1, y).left_wall;

    if (y + 1 < maze.height)
        result.down = !maze_at(maze, x + 1, y).top_wall;
    
    return result;
}

void maze_print(Maze maze) {
    const char* cells[4][2] = {
        {"+ ", "  "}, // left_wall: false, top_wall: false,
        {"+ ", "| "}, // left_wall: true, top_wall: false,
        {"+-", "  "}, // left_wall: false, top_wall: true,
        {"+-", "| "}, // left_wall: true, top_wall: true,
    };

    for (size_t y = 0; y < maze.height; y++) {
        for (size_t row = 0; row < 2; row++) {
            for (size_t x = 0; x < maze.width; x++) {
                MazeCell cell = maze_at(maze, x, y);

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

    for (size_t x = 0; x < maze.width; x++)
        printf("+-");
    printf("+\n");
}