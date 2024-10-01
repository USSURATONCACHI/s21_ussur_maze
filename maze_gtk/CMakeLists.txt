add_executable(maze_gtk
    src/main.c
)

target_include_directories(maze_gtk PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_directories(maze_gtk PRIVATE ${GTK3_LIBRARY_DIRS})

target_include_directories(maze_gtk PRIVATE include)
target_link_libraries(maze_gtk PUBLIC libmaze ${GTK3_LIBRARIES})


install(
    TARGETS maze_gtk
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)