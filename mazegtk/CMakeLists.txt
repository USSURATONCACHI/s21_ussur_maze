find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(Epoxy REQUIRED epoxy)

add_executable(s21_ussur_mazegtk
    src/main.c
    src/maze_app.c
    src/maze_app_create.c
    src/viewmodel.c
    src/viewmodel_state.c
    
    src/model/camera.c
    
    src/controller/camera.c

    src/view/gtk_view_create.c
    src/view/gtk_view_free.c
    src/view/gtk_view_gl_realize.c

    src/util/vec_g_error.c
    src/util/error_list.c
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