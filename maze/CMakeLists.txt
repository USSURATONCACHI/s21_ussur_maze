add_library(s21_ussur_maze SHARED
    src/maze.c
)
target_include_directories(s21_ussur_maze PUBLIC include)
target_include_directories(s21_ussur_maze PRIVATE pinclude)

install(
    TARGETS s21_ussur_maze
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)