add_library(s21_ussur_maze SHARED
    src/maze.c
)
target_include_directories(s21_ussur_maze PUBLIC include)

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