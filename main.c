#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
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

    var t = start_stopwatch();

    var str = fstr_from_C(":)");

//    fstr_append_format_C(str, "~~~~~~%d~~~~~~%f", 10, 1.24);

    stop_stopwatch(t);

    fstr_slowprint(str);

    var lineStr = fstr_from_C("");
    fstr_pad(lineStr, 10, '-', 0);

    printf("\n");
    fstr_slowprint(lineStr);
    printf("\nElapsed: %ldms", t.tv_usec / 1000);

    return 0;
}