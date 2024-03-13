#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include "fstr.h"

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
    var str = fstr_from_format_C("(%d)(%d)(%d)", 10, 20, 30);

    int ITERATIONS = 1000;

    var tv = start_stopwatch();
    for (int i = 0; i < ITERATIONS; i++) {
        fstr_print_slow(str);
    }
    var slowPrint = stop_stopwatch(tv);

    tv = start_stopwatch();

    for (int i = 0; i < ITERATIONS; i++) {
        fstr_print(str);
    }

    var fastPrint = stop_stopwatch(tv);

    printf("Slow Print: %fms\nFast Print: %fms", slowPrint, fastPrint);

    return 0;
}