enable_testing()

add_executable(test
    src/test.cpp
)
target_link_libraries(test GTest::gtest GTest::gtest_main libmaze)
install(
    TARGETS test
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

include(GoogleTest)
gtest_discover_tests(test)