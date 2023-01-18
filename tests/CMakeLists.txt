project(${PROJECT_NAME}_Tests)

add_subdirectory(googletest)

# collecting .cpp files in folder tests
file(GLOB_RECURSE TEST_SOURCE_FILES CONFIGURE_DEPENDS *.cpp)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${CMAKE_PROJECT_NAME} gtest_main)
target_sources(${PROJECT_NAME} PRIVATE ${TEST_SOURCE_FILES})

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
