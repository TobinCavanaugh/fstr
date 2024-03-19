#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <locale.h>
#include "fstr.h"
#include "fstr_parse.h"

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

    /*
    var intStr = fstr_from_C("-1234");

    long res = 0;
    if (!fstr_try_to_long(intStr, &res)) {
        printf("failed :(\n");
    }

    printf("%ld", res);


    return 0;

     */

    var str = fstr_from_C("010101010101010--2");

    var other = fstr_copy(str);

    fstr_remove_chr_varargs(other, 3, '1', '0', '-');

    fstr_println(str);
    fstr_println(other);

    return 0;
}