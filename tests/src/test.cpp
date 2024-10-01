#include <gtest/gtest.h>

extern "C" {
    #include <libmaze/maze.h>
}

TEST(Test, Test) {
    hello_maze();
    EXPECT_EQ(1 + 1, 2);
}