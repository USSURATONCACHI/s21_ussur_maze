#include <libmaze/maze.h>

void MzMaze_print(const MzMaze* maze) {
    const char* cells[4][2] = {
        {"+ ", "  "},
        {"+ ", "| "},
        {"+-", "  "},
        {"+-", "| "},
    };

    for (size_t y = 0; y < maze->height; y++) {
        for (size_t row = 0; row < 2; row++) {
            for (size_t x = 0; x < maze->width; x++) {
                MzCell cell = MzMaze_at(maze, x, y);

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

void MzMaze_print_with_path(const MzMaze* maze, const vec_MzCellPos* path) {
    for (size_t y = 0; y < maze->height; y++) {
        for (size_t row = 0; row < 2; row++) {
            for (size_t x = 0; x < maze->width; x++) {
                char cell_symbol = ' ';
                for (size_t path_i = 0; path_i < path->length; path_i++) {
                    MzCellPos pos = path->data[path_i];
                    if (pos.x == x && pos.y == y) {
                        cell_symbol = '*';
                    }
                }
                
                MzCell cell = MzMaze_at(maze, x, y);

                if (x == 0) cell.left_wall = true;
                if (y == 0) cell.top_wall = true;

                if (row == 0) {
                    printf("%c%c", '+', cell.top_wall ? '-' : ' ');
                } else {
                    printf("%c%c", cell.left_wall ? '|' : ' ', cell_symbol);
                }
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