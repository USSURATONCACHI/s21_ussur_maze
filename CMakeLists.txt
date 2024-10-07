cmake_minimum_required(VERSION 3.5)
project(s21_maze VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For VSCode clang extension integraion
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ggdb -O3")

add_subdirectory(tests)
set(BCSTD_BUILD_EXAMPLES OFF)
add_subdirectory(better_c_std)
add_subdirectory(opengl_utils)
add_subdirectory(maze)
add_subdirectory(mazegtk)
# add_subdirectory(mazectl)
