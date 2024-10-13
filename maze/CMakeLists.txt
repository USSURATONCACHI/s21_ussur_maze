add_library(s21_ussur_maze SHARED
    src/maze.c
    src/cell_pos.c
    src/a_star.c
    src/vec_size_t.c
    src/ellers_algorithm.c
    src/ellers_offthread_generator.c
    src/ellers_resources.c
)
target_include_directories(s21_ussur_maze PUBLIC include)

target_link_libraries(s21_ussur_maze PUBLIC s21_ussur_better_c_std)

include(GNUInstallDirs)
install(
    TARGETS s21_ussur_maze
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install the header files
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)