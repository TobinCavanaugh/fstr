#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "fstr.h"
#include "fstr_parse.h"
#include "fstr_tests.h"

#define var __auto_type

//TODO MEMORY ARENA MACROS, OVERRIDE MALLOC, IMPLY ENDING THING

struct timeval start_stopwatch() {
    struct timeval tv = {0, 0};
    gettimeofday(&tv, NULL);
    return tv;
}

double stop_stopwatch(struct timeval start_time) {
    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0; // seconds to milliseconds
    elapsed_time += (end_time.tv_usec - start_time.tv_usec) / 1000.0; // microseconds to milliseconds

    return elapsed_time;
}

int main() {
//    fstr_run_tests();

    fstr *str = fstr_from_C(" \t\n   0 12345 6 7 89  \n\n ");
    fstr_println(str);
    printf("===========\n");
    fstr_trim(str, 0);

    fstr_println(str);


    return 0;
}