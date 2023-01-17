cmake_minimum_required (VERSION 3.20)

project(SudokuSolver)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC "includes/")
target_sources(${PROJECT_NAME} PRIVATE "src/Sudoku.cpp")

enable_testing()
add_subdirectory(tests)
