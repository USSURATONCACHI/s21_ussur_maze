find_package(PkgConfig REQUIRED)
pkg_check_modules(Epoxy REQUIRED epoxy)

add_library(s21_ussur_opengl_utils SHARED
    src/framebuffer.c
    src/gl_program.c
    src/mesh.c
    src/shader.c
)

target_include_directories(s21_ussur_opengl_utils PRIVATE ${EPOXY_INCLUDE_DIRS})
target_link_directories   (s21_ussur_opengl_utils PRIVATE ${EPOXY_LIBRARY_DIRS})
target_link_libraries     (s21_ussur_opengl_utils PUBLIC  ${EPOXY_LIBRARIES})

target_link_libraries     (s21_ussur_opengl_utils PUBLIC  s21_ussur_better_c_std)

target_include_directories(s21_ussur_opengl_utils PUBLIC include)

include(GNUInstallDirs)
install(
    TARGETS s21_ussur_opengl_utils
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