#include <stdio.h>
#include "fstr.h"
#include "time.h"
#include "sys/time.h"
#include "fstr_convert.h"

struct timeval start_stopwatch()
{
    struct timeval tv = {0, 0};
    gettimeofday(&tv, NULL);
    return tv;
}

double stop_stopwatch(struct timeval start_time)
{
    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0; // seconds to milliseconds
    elapsed_time += (end_time.tv_usec - start_time.tv_usec) / 1000.0; // microseconds to milliseconds

    return elapsed_time;
}

usize fsize(FILE* fp)
{
    fseek(fp, 0, SEEK_END);
    usize size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return size;
}

void BigFileTest()
{
    FILE* f = fopen("C:\\Users\\tobin\\Documents\\big_smaller.txt", "r");
    usize size = fsize(f);

    fstr* str = fstr_from_length(size, ' ');

    usize i = 0;
    chr c;
    while ((c = (chr)getc(f)) != EOF)
    {
        str->data[i++] = c;
    }

    fclose(f);

    //    fstr_count_C(str, "The ")

    char* search = "The ";
    char* replace = "---";

    __auto_type sw = start_stopwatch();
    fstr_count_C(str, search);
    double ctime = stop_stopwatch(sw);

    sw = start_stopwatch();
    fstr_replace_C(str, search, replace);
    double rtime = stop_stopwatch(sw);

    //    fstr_println(str);

    printf("Count: %fms | Replace: %fms\n", ctime, rtime);


    fstr_free(str);
}

#define ANSI_BACKGROUND_COLOR_ESCAPE "\x1b[48;2;%03d;%03d;%03dm"
#define ANSI_COLOR_RESET "\x1b[0m"

int main()
{
    /*
    int i;
    for (i = 0; i < 10; i++) {
        BigFileTest();
    }
    return 0;
     */


    /*
        fstr *val = fstr_from_length(90, ' ');

        __auto_type sw = start_stopwatch();

        int i = 0;
        for (; i < 1000000; i++) {
            memcpy(val->data,
                            "001234567890123456789012345678901234567890123456789012345678901234567890123456789123456789",
                            90);
        }

        printf("%fms", stop_stopwatch(sw));

        return 1;
     */

    //====================================================================


    {
        fstr* str = fstr_from_C("123_123_123");
        fstr_println(str);
        fstr_remove_at(str, 0, 3);
        fstr_println(str);
        fstr_free(str);
    }
    fstr_println(NULL);
    {
        fstr* str = fstr_from_C("the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the");
        fstr_println(str);
        fstr_replace_C(str, "the", "!");
        fstr_println(str);
        fstr_free(str);
    }
    fstr_println(NULL);
    {
        fstr* str = fstr_from_C(
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
        fstr* str = fstr_from_C(
            "_________________________________________________________________________________________________________________________________");
        fstr_println(str);
        fstr_replace_C(str, "___", "^");
        fstr_println(str);
        fstr_free(str);
    }
    fstr_println(NULL);
    {
        fstr* str = fstr_from_C("0123456789");
        fstr_println(str);
        fstr_remove_at(str, 0, 5);
        fstr_print_chrs(str);
        printf("\n");
        fstr_free(str);
    }
    fstr_println(NULL);
    {
        fstr* str = fstr_from_C("999999999");

        fstr_println(str);
        printf("%lld\n", fstr_count_C(str, "999"));

        fstr_free(str);
    }
    fstr_println(NULL);
    {
        fstr* str = fstr_from_C("999999999");

        fstr_println(str);
        printf("%lld\n\n", fstr_count_C(str, "9"));

        fstr_free(str);
    }
    {
        fstr* str = fstr_from_C("10.512512561616");

        printf("%f\n\n", fstr_to_double(str).f_val);

        fstr_free(str);
    }

    {
        fstr* str = fstr_from_C("000.10001");

        printf("%f\n\n", fstr_to_double(str).f_val);

        fstr_free(str);
    }

    {
        fstr* str = fstr_from_C("1");

        printf("%f\n\n", fstr_to_double(str).f_val);

        fstr_free(str);
    }

    {
        fstr* str = fstr_from_C("010101110111010");

        printf("%llu\n\n", fstr_u64_from_bin(str).u_val);

        fstr_free(str);
    }

    {
        fstr* str = fstr_from_C("0x0101010001");

        printf("%llu\n\n", fstr_u64_from_bin(str).u_val);

        fstr_free(str);
    }

    {
        fstr* str = fstr_from_C("ABCDEF");
        fstr_print_bin(str);
        fstr_println(NULL);
        fstr_print_hex(str);
        fstr_println(NULL);
        fstr_print_num(str);


        fstr_free(str);
    }
}
