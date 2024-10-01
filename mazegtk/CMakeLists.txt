add_executable(s21_ussur_mazegtk
    src/main.c
)

target_include_directories(s21_ussur_mazegtk PRIVATE ${GTK3_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
target_link_directories(s21_ussur_mazegtk PRIVATE ${GTK3_LIBRARY_DIRS} ${GLIB_LIBRARY_DIRS} ${GIO_LIBRARY_DIRS})

target_include_directories(s21_ussur_mazegtk PRIVATE include)
target_link_libraries(s21_ussur_mazegtk PUBLIC s21_ussur_maze ${GTK3_LIBRARIES} ${GLIB_LIBRARIES} ${GIO_LIBRARIES})


install(
    TARGETS s21_ussur_mazegtk
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# === Resources ====
file(GLOB RECURSE GRESOURCE_FILES "resources/*")
set(GRESOURCE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/s21_ussur_mazegtk.gresource)

add_custom_command(
    OUTPUT ${GRESOURCE_OUTPUT}
    COMMAND glib-compile-resources --target=${GRESOURCE_OUTPUT} resources.xml
    DEPENDS ${RESOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/resources
    COMMENT "Compiling gresource xml file to ${GRESOURCE_OUTPUT}"
)
add_custom_target(s21_ussur_mazegtk_resources ALL DEPENDS ${GRESOURCE_OUTPUT})

install(
    FILES ${GRESOURCE_OUTPUT}
    DESTINATION share
)
# === Resources ====