cmake_minimum_required(VERSION 3.5)
project(s21_maze VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For VSCode clang extension integraion
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ggdb")

add_subdirectory(third_party)


add_subdirectory(bmp_reader)
add_subdirectory(console_bmp)
add_subdirectory(tests)
