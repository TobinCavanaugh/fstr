#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "fstr.h"


#define var __auto_type

//TODO MEMORY ARENA MACROS, OVERRIDE MALLOC, IMPLY ENDING THING




void start_stopwatch(struct timeval *start_time) {
    gettimeofday(start_time, NULL);
}

double stop_stopwatch(struct timeval start_time) {
    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0; // seconds to milliseconds
    elapsed_time += (end_time.tv_usec - start_time.tv_usec) / 1000.0; // microseconds to milliseconds
    return elapsed_time;
}

int main() {

    var str1 = fstr_from_C("|");

    for (int i = 0; i <= 2; i++) {

        printf("[");
        var str2 = fstr_from_C("--");
        fstr_insert(str2, str1, i);

        fstr_slowprint(str2);
        fstr_free(str2);

        printf("]");
    }

//    for (int i = 0; i < 50; i++) {
//        var fstr_string = fstr_from_C("ABC");
//        fstr_pad(fstr_string, i, '_', 0);
//        fstr_slowprint(fstr_string);
//        printf("\n");
//    }

    return 0;

    int F_ITER = 10;
    int BENCHMARK_ITER = 1;
    char *ADDITION = "AA";

    for (int i = 0; i < BENCHMARK_ITER; i++) {

        struct timeval start_time = {};
        start_stopwatch(&start_time);

        var myStr = fstr_from_C("");

        for (uint64_t r = 0; r < F_ITER; r++) {
            fstr_append_C(myStr, ADDITION);
        }

        var fstrTime = stop_stopwatch(start_time);

//        fstr_slowprint(myStr);

        start_stopwatch(&start_time);

        char *cStr = malloc(1 * sizeof(char));
        cStr[0] = '0';

        for (int r = 0; r < F_ITER; r++) {
            cStr = realloc(cStr, strlen(cStr) + strlen(ADDITION) + 2);
            cStr[r] = '\0';
            strcat(cStr, ADDITION);
        }

        cStr[F_ITER - 1] = '\0';


        var cTime = stop_stopwatch(start_time);
        printf("%.3fms - %.3fms == %.3fms\n", fstrTime, cTime, fstrTime - cTime);

        printf("\nFSTR:");
        fstr_slowprint(myStr);

        printf("\n\n\n");

        printf("\nCSTR:");
        printf("%s", cStr);

        fstr_free(myStr);
        free(cStr);

        printf("\n----------------------------------------------------------------------\n");
    }


    return 0;
}
