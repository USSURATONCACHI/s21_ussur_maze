enable_testing()

find_package(GTest REQUIRED)

add_executable(s21_ussur_maze_test
    src/test.cpp
    src/a_star.cpp
)
target_link_libraries(s21_ussur_maze_test GTest::gtest GTest::gtest_main s21_ussur_maze)
install(
    TARGETS s21_ussur_maze_test
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

include(GoogleTest)
gtest_discover_tests(s21_ussur_maze_test)