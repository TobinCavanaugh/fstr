#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
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

    int c;
    for (c = 0; c <= 255; c++) {
        printf("%c | _%c | ^%c\n", c, chr_to_lower(c), chr_to_upper(c));
    }

    return 0;

    int v;
    for (v = 0; v <= 255; v++) {
        char c = (char) v;
        printf("%c > _%c & ^%c\n", c, chr_to_lower(c), chr_to_upper(c));
    }

//    var str = fstr_from_C("~~~~____~~~~");
//
//    var other = fstr_copy(str);
//
//    fstr_remove_C(other, "~~~~");
//    fstr_remove_chr_varargs(other, 3, '.', '-', ' ');

//    fstr_println(str);
//    fstr_println(other);
//
    return 0;
}