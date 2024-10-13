#include <gtest/gtest.h>

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #include <libmaze/maze.h>
    #include <libmaze/a_star.h>
    #include <better_c_std/prettify.h>
}

TEST(MzAStar, Test1) {
    MzMazeResult res = MzMaze_create_perfect_eller(10, 10);
    ASSERT_TRUE(res.is_ok);
    MzMaze maze = res.ok;

    MzMaze_print(&maze);

    MzAStarPathfinder pathfinder = MzAStarPathfinder_create(0, 0, 9, 9);

    vec_MzCellPos path = MzAStarPathfinder_pathfind(&pathfinder, &maze);

    for (size_t i = 0; i < path.length; i++) {
        MzCellPos pos = path.data[i];
        debugln("Path %zu : X %zu Y %zu", i, pos.x, pos.y);
    }
    MzMaze_print_with_path(&maze, &path);

    vec_MzCellPos_free(path);
    MzAStarPathfinder_free(pathfinder);
    MzMaze_free(maze);
}