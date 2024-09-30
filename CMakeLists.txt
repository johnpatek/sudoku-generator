cmake_minimum_required(VERSION 3.10)

project(sudoku-generator)

add_executable(generator main.cpp)

target_compile_options(generator PRIVATE "/std:c++17")