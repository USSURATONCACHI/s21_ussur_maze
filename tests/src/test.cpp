#include <gtest/gtest.h>

extern "C" {
    #include <libmaze/maze.h>
}

TEST(Maze, Create) {
    Maze maze = maze_create(150, 151);

    EXPECT_EQ(maze.width, 150);
    EXPECT_EQ(maze.height, 151);
}


TEST(Maze, WritePersists) {
    Maze maze = maze_create(2, 2);

    for (size_t x = 0; x < maze.width; x++) {
        for (size_t y = 0; y < maze.height; y++) {
            maze_set_at(maze, x, y, (MazeCell) {
                .top_wall = (x & 1) ? true : false,
                .left_wall = (x & 6) ? true : false
            });
        }
    }

    for (size_t x = 0; x < maze.width; x++) {
        for (size_t y = 0; y < maze.height; y++) {
            MazeCell cell = maze_at(maze, x, y);
            EXPECT_EQ(cell.top_wall, (x & 1) ? true : false);
            EXPECT_EQ(cell.left_wall, (x & 6) ? true : false);
        }
    }
}

TEST(Maze, Print) {
    Maze maze = maze_create(9, 4);

    maze_set_at(maze, 1, 1, (MazeCell) {
        .top_wall = true,
        .left_wall = true,
    });
    maze_set_at(maze, 2, 1, (MazeCell) {
        .top_wall = true,
        .left_wall = false,
    });
    maze_set_at(maze, 1, 2, (MazeCell) {
        .top_wall = true,
        .left_wall = false,
    });

    EXPECT_EQ(maze.width, 9);
    EXPECT_EQ(maze.height, 4);
    maze_print(maze);
}

// TEST(Maze, print) {
//     Maze maze = maze_create(6, 6);

//     EXPECT_EQ(maze.width, 6);
//     EXPECT_EQ(maze.height, 6);

//     // maze_print(maze);
// }