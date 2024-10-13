#include <gtest/gtest.h>

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #include <libmaze/maze.h>
    #include <libmaze/a_star.h>
}

TEST(MzAStar, Test1) {
    MzMazeResult res = MzMaze_create_perfect_eller(5, 5);
    ASSERT_TRUE(res.is_ok);
    MzMaze maze = res.ok;

    MzMaze_print(&maze);

    MzAStarPathfinder pathfinder = MzAStarPathfinder_create(0, 0, 4, 4);

    MzAStarPathfinder_pathfind(&pathfinder, &maze);

    MzAStarPathfinder_free(pathfinder);
    MzMaze_free(maze);
}