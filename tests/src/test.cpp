#include "libmaze/directions_struct.h"
#include <gtest/gtest.h>

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #include <libmaze/maze.h>
}

TEST(MzMaze, Create) {
    MzMazeResult res = MzMaze_create(150, 151);
    ASSERT_TRUE(res.is_ok);
    MzMaze maze = res.ok;

    EXPECT_EQ(maze.width, 150);
    EXPECT_EQ(maze.height, 151);

    MzMaze_free(maze);
}


TEST(MzMaze, WritePersists) {
    MzMazeResult res = MzMaze_create(2, 2);
    ASSERT_TRUE(res.is_ok);
    MzMaze maze = res.ok;

    for (size_t x = 0; x < maze.width; x++) {
        for (size_t y = 0; y < maze.height; y++) {
            MzMaze_set_at(&maze, x, y, (MzCell) {
                .top_wall = (x & 1) ? true : false,
                .left_wall = (x & 6) ? true : false
            });
        }
    }

    for (size_t x = 0; x < maze.width; x++) {
        for (size_t y = 0; y < maze.height; y++) {
            MzCell cell = MzMaze_at(&maze, x, y);
            EXPECT_EQ(cell.top_wall, (x & 1) ? true : false);
            EXPECT_EQ(cell.left_wall, (x & 6) ? true : false);
        }
    }
    MzMaze_free(maze);
}

TEST(MzMaze, Print) {
    MzMazeResult res = MzMaze_create(9, 4);
    ASSERT_TRUE(res.is_ok);
    MzMaze maze = res.ok;

    MzMaze_set_at(&maze, 1, 1, (MzCell) {
        .top_wall = true,
        .left_wall = true,
    });
    MzMaze_set_at(&maze, 2, 1, (MzCell) {
        .top_wall = true,
        .left_wall = false,
    });
    MzMaze_set_at(&maze, 1, 2, (MzCell) {
        .top_wall = true,
        .left_wall = false,
    });
    MzMaze_print(&maze);
    MzMaze_free(maze);
}

TEST(MzMaze, Directions) {
    MzMazeResult res = MzMaze_create(9, 4);
    ASSERT_TRUE(res.is_ok);
    MzMaze maze = res.ok;

    MzMaze_set_at(&maze, 1, 1, (MzCell) {
        .top_wall = true,
        .left_wall = true,
    });
    MzMaze_set_at(&maze, 2, 1, (MzCell) {
        .top_wall = true,
        .left_wall = false,
    });
    MzMaze_set_at(&maze, 1, 2, (MzCell) {
        .top_wall = true,
        .left_wall = false,
    });

    MzDirections dir = MzMaze_where_can_go(&maze, 1, 1);
    EXPECT_FALSE(dir.up);
    EXPECT_FALSE(dir.down);
    EXPECT_FALSE(dir.left);
    EXPECT_TRUE(dir.right);

    MzMaze_free(maze);
}

TEST(MzMaze, Random) {
    MzMazeResult res = MzMaze_create(9, 6);
    ASSERT_TRUE(res.is_ok);
    MzMaze maze = res.ok;

    srand(time(NULL));
    MzMaze_fill_random(&maze);
    MzMaze_print(&maze);
    MzMaze_free(maze);
}