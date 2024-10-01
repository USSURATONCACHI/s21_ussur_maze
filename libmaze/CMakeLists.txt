add_library(libmaze SHARED
    src/maze.c
)
target_include_directories(libmaze PUBLIC include)
target_include_directories(libmaze PRIVATE pinclude)

install(
    TARGETS libmaze
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)