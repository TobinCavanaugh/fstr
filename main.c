#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <locale.h>
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

    var str = fstr_from_C(L"0123456789");

    fstr_print(str);
    printf("\n---\n");

    fstr_append_C(str, L"\t");
    fstr_append_C(str, L":)");

    printf("~~~\n");


//    fstr_append_C(str, ":)");
    fstr_print(str);

    var heap = fstr_as_C_heap(str);
    printf("\n%s", heap);

//    fstr_append_C(str, L":)");


    return 0;
}