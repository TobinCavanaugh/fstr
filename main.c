#include <stdio.h>
#include "fstr.h"
#include "fstr_parse.h"
#include "stdint.h"

int main() {
    fstr *str = fstr_from_C(" 1234 ");
    int64_t value = 0;
    if (fstr_try_to_i64(str, &value)) {
        printf("%lld", value);
    } else {
        printf("Failed to parse fstr: ");
        fstr_print(str);
    }
}

/*
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

 */