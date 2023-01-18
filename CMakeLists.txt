cmake_minimum_required (VERSION 3.20)

project(SudokuSolver)

set(CMAKE_CXX_STANDARD 20)

# collecting .cpp files in folder src
file(GLOB_RECURSE MAIN_LIB_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC "includes/")
target_sources(${PROJECT_NAME} PRIVATE ${MAIN_LIB_SOURCE_FILES})

enable_testing()
add_subdirectory(tests)
