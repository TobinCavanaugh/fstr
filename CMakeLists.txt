cmake_minimum_required(VERSION 3.26)
project(fstr C)

set(CMAKE_C_STANDARD 99)

add_executable(fstr main.c
        fstr.c)
