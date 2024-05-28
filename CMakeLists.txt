cmake_minimum_required(VERSION 3.26)
project(fstr C)

set(CMAKE_C_STANDARD 90)

add_executable(fstr
        main.c
        fstr.c
        fstr_convert.c
        fstr_tests.c
        fstr_convert.h
)