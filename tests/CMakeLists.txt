project(${PROJECT_NAME}_Tests)

add_subdirectory(googletest)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${CMAKE_PROJECT_NAME} gtest_main)
target_sources(${PROJECT_NAME} PRIVATE "StaticRegularSudokuTest.cpp")

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
