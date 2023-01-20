set(TEST_SOURCES
        script/test_basic.cpp
        test_basiccolor.cpp
        test_position.cpp
        test_rect.cpp
        test_result.cpp
        test_size.cpp)

add_executable(${PROJECT_NAME}_test ${TEST_SOURCES} ${SCRIPT_SOURCES})
target_link_libraries(${PROJECT_NAME}_test fmt::fmt raylib GTest::gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)
