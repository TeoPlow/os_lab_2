cmake_minimum_required(VERSION 3.10)

project(OS/lab_01)

set(CMAKE_C_STANDARD 11)

add_executable(child child.c)

set(REQUIRED_FILES child)

add_executable(main main.c)