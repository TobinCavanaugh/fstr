cmake_minimum_required(VERSION 3.26)
project(fstr C)

set(CMAKE_C_STANDARD 90)

add_executable(fstr
#        main.c
        fstr.c
        fstr_parse.c
        fstr_tests.c
        #        examples/USING_WCHAR_EX.c
#        examples/USING_CHAR_EX.c
#        examples/chr_is_upper.c
#        examples/chr_is_lower.c
#        examples/chr_is_alpha.c
#        examples/chr_to_lower.c
#        examples/chr_to_upper.c
        examples/from_C.c

)