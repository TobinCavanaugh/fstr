#include <stdio.h>
#include <malloc.h>
#include "fstr.h"
#include "time.h"
#include "sys/time.h"
#include "fstr_parse.h"


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

void dofileread() {

    FILE *f = fopen("C:\\Users\\tobin\\Documents\\big.txt", "r");
    int64_t size = fsize(f);

    fstr *str = fstr_from_length(size, ' ');

    usize i = 0;
    chr c;
    while ((c = getc(f)) != EOF) {
        str->data[i++] = c;
    }

    fclose(f);

    __auto_type sw = start_stopwatch();
    fstr_replace_C(str, "the", "---");
//    fstr_result res = fstr_index_of_C(str, "The");


    double time = stop_stopwatch(sw);

//    printf("%d -=> %lld\n", res.success, res.u_val);
    printf("%fms", time);
}

int main() {
    {
        fstr *str = fstr_from_C("the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_the_");
        fstr_println(str);
        fstr_replace_C(str, "the", "_");
        fstr_println(str);
        fstr_free(str);
    }
    fstr_println(NULL);
    {
        fstr *str = fstr_from_C("___");
        fstr_println(str);
        fstr_replace_C(str, "___", "______");
        fstr_println(str);
    }
    fstr_println(NULL);
    {
//        fstr * str = fstr_from_C()
    }
}