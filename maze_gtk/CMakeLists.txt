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

# === Resources ====
file(GLOB RECURSE GRESOURCE_FILES "resources/*")
set(GRESOURCE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/maze_gtk.gresource)

add_custom_command(
    OUTPUT ${GRESOURCE_OUTPUT}
    COMMAND glib-compile-resources --target=${GRESOURCE_OUTPUT} resources.xml
    DEPENDS ${RESOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/resources
    COMMENT "Compiling gresource xml file to ${GRESOURCE_OUTPUT}"
)
add_custom_target(maze_gtk_resources ALL DEPENDS ${GRESOURCE_OUTPUT})

install(
    FILES ${GRESOURCE_OUTPUT}
    DESTINATION share
)
# === Resources ====