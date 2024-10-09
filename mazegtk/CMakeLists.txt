find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(Epoxy REQUIRED epoxy)

add_executable(s21_ussur_mazegtk
    src/main.c
    
    src/model/model.c
    
    src/controller/camera.c
    src/controller/controller.c
    src/controller/maze.c
    src/controller/maze_gen.c

    src/view_gtk/subviews/camera_controls_view.c
    src/view_gtk/subviews/camera_settings_view.c
    src/view_gtk/subviews/dropdown_view.c
    src/view_gtk/subviews/grabby_cursor_view.c
    src/view_gtk/subviews/maze_gen_view.c
    src/view_gtk/subviews/gl_maze_view.c

    src/view_gtk/gtk_view_create.c
    src/view_gtk/gtk_view_gl_area.c
    src/view_gtk/gtk_view.c
    src/view_gtk/maze_ssbo.c
)

target_include_directories(s21_ussur_mazegtk PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_directories   (s21_ussur_mazegtk PRIVATE ${GTK3_LIBRARY_DIRS})
target_link_libraries     (s21_ussur_mazegtk PUBLIC  ${GTK3_LIBRARIES})

target_include_directories(s21_ussur_mazegtk PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_directories   (s21_ussur_mazegtk PRIVATE ${GLIB_LIBRARY_DIRS})
target_link_libraries     (s21_ussur_mazegtk PUBLIC  ${GLIB_LIBRARIES})

target_include_directories(s21_ussur_mazegtk PRIVATE ${GIO_INCLUDE_DIRS})
target_link_directories   (s21_ussur_mazegtk PRIVATE ${GIO_LIBRARY_DIRS})
target_link_libraries     (s21_ussur_mazegtk PUBLIC  ${GIO_LIBRARIES})

target_include_directories(s21_ussur_mazegtk PRIVATE ${Epoxy_INCLUDE_DIRS})
target_link_directories   (s21_ussur_mazegtk PRIVATE ${Epoxy_LIBRARY_DIRS})
target_link_libraries     (s21_ussur_mazegtk PUBLIC  ${Epoxy_LIBRARIES})

target_link_libraries     (s21_ussur_mazegtk PUBLIC s21_ussur_maze s21_ussur_opengl_utils m)

target_include_directories(s21_ussur_mazegtk PRIVATE include)
target_link_options(s21_ussur_mazegtk PRIVATE "-rdynamic")

install(
    TARGETS s21_ussur_mazegtk
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# === Resources ====
file(GLOB_RECURSE GRESOURCE_FILES "resources/*")

# Filter out tmp files created by Glade
foreach(file ${GRESOURCE_FILES})
    if(file MATCHES "~$" OR file MATCHES "^~" OR file MATCHES "^#")
        list(REMOVE_ITEM GRESOURCE_FILES ${file})
    endif()
endforeach()

set(GRESOURCE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/s21_ussur_mazegtk.gresource)

add_custom_command(
    OUTPUT ${GRESOURCE_OUTPUT}
    COMMAND glib-compile-resources --target=${GRESOURCE_OUTPUT} resources.xml
    DEPENDS ${GRESOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/resources
    COMMENT "Compiling gresource xml file to ${GRESOURCE_OUTPUT}"
)
add_custom_target(s21_ussur_mazegtk_resources ALL DEPENDS ${GRESOURCE_OUTPUT})

install(
    FILES ${GRESOURCE_OUTPUT}
    DESTINATION share
)
# === Resources ====