#include <stdio.h>
#include <malloc.h>
#include "fstr.h"
#include "time.h"
#include "sys/time.h"
#include "fstr_parse.h"
#include "fstr_tests.h"


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

usize fsize(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    usize size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return size;
}

void BigFileTest() {

    FILE *f = fopen("C:\\Users\\tobin\\Documents\\big_smaller.txt", "r");
    usize size = fsize(f);

    fstr *str = fstr_from_length(size, ' ');

    usize i = 0;
    chr c;
    while ((c = (chr) getc(f)) != EOF) {
        str->data[i++] = c;
    }

    fclose(f);

    __auto_type sw = start_stopwatch();
    fstr_replace_C(str, "The", "---");
    double time = stop_stopwatch(sw);

//    fstr_println(str);

    printf("%fms\n", time);

    fstr_free(str);
}

int main() {

//    /*

    int i;
    for (i = 0; i < 10; i++) {
        BigFileTest();
    }
//    return 0;
//     */


/*
    fstr *val = fstr_from_length(90, ' ');

    __auto_type sw = start_stopwatch();

    int i = 0;
    for (; i < 1000000; i++) {
        memcpy_internal(val->data,
                        "001234567890123456789012345678901234567890123456789012345678901234567890123456789123456789",
                        90);
    }

    printf("%fms", stop_stopwatch(sw));

    return 1;
 */

    //====================================================================


    {
        fstr *str = fstr_from_C("123_123_123");
        fstr_println(str);
        fstr_remove_at(str, 0, 3);
        fstr_println(str);
        fstr_free(str);
    }
    fstr_println(NULL);
    {
        fstr *str = fstr_from_C("the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the");
        fstr_println(str);
        fstr_replace_C(str, "the", "!");
        fstr_println(str);
        fstr_free(str);
    }
    fstr_println(NULL);
    {
        fstr *str = fstr_from_C(
                "the quick brown fox jumps over the lazy dog. Pack my box with five dozen liquor jugs. Jinxed wizards pluck ivy from the big quilt. Jaded zombies acted quaintly but kept driving their oxen forward");
        fstr_println(str);
        fstr_replace_C(str, "x", "!!!!");
        fstr_replace_C(str, "o", "----");
        fstr_replace_C(str, "t", "/");
        fstr_println(str);
        fstr_free(str);
    }
    fstr_println(NULL);
    {
        fstr *str = fstr_from_C(
                "_________________________________________________________________________________________________________________________________");
        fstr_println(str);
        fstr_replace_C(str, "___", "^");
        fstr_println(str);
        fstr_free(str);
    }
    fstr_println(NULL);
    {
        fstr *str = fstr_from_C("0123456789");
        fstr_println(str);
        fstr_remove_at(str, 0, 5);
        fstr_print_chrs(str);
        fstr_free(str);
    }

    fstr_println(NULL);
}