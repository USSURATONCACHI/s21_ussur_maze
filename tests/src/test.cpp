#include "libmaze/directions_struct.h"
#include <gtest/gtest.h>

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #include <libmaze/maze.h>
}

TEST(MzMaze, Create) {
    MzMaze maze = mz_maze_create(150, 151);

    EXPECT_EQ(maze.width, 150);
    EXPECT_EQ(maze.height, 151);

    mz_maze_free(maze);
}


TEST(MzMaze, WritePersists) {
    MzMaze maze = mz_maze_create(2, 2);

    for (size_t x = 0; x < maze.width; x++) {
        for (size_t y = 0; y < maze.height; y++) {
            mz_maze_set_at(&maze, x, y, (MzCell) {
                .top_wall = (x & 1) ? true : false,
                .left_wall = (x & 6) ? true : false
            });
        }
    }

    for (size_t x = 0; x < maze.width; x++) {
        for (size_t y = 0; y < maze.height; y++) {
            MzCell cell = mz_maze_at(&maze, x, y);
            EXPECT_EQ(cell.top_wall, (x & 1) ? true : false);
            EXPECT_EQ(cell.left_wall, (x & 6) ? true : false);
        }
    }
    mz_maze_free(maze);
}

TEST(MzMaze, Print) {
    MzMaze maze = mz_maze_create(9, 4);

    mz_maze_set_at(&maze, 1, 1, (MzCell) {
        .top_wall = true,
        .left_wall = true,
    });
    mz_maze_set_at(&maze, 2, 1, (MzCell) {
        .top_wall = true,
        .left_wall = false,
    });
    mz_maze_set_at(&maze, 1, 2, (MzCell) {
        .top_wall = true,
        .left_wall = false,
    });
    mz_maze_print(&maze);
    mz_maze_free(maze);
}

TEST(MzMaze, Directions) {
    MzMaze maze = mz_maze_create(9, 4);

    mz_maze_set_at(&maze, 1, 1, (MzCell) {
        .top_wall = true,
        .left_wall = true,
    });
    mz_maze_set_at(&maze, 2, 1, (MzCell) {
        .top_wall = true,
        .left_wall = false,
    });
    mz_maze_set_at(&maze, 1, 2, (MzCell) {
        .top_wall = true,
        .left_wall = false,
    });

    MzDirections dir = mz_maze_where_can_go(&maze, 1, 1);
    EXPECT_FALSE(dir.up);
    EXPECT_FALSE(dir.down);
    EXPECT_FALSE(dir.left);
    EXPECT_TRUE(dir.right);

    mz_maze_free(maze);
}

TEST(MzMaze, Random) {
    MzMaze maze = mz_maze_create(9, 6);
    srand(time(NULL));
    mz_maze_fill_random(&maze);
    mz_maze_print(&maze);
    mz_maze_free(maze);
}