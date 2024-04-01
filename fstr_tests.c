//
// Created by tobin on 3/20/2024.
//

#include <stdio.h>
#include <malloc.h>
#include "fstr_tests.h"
#include "fstr.h"

#define linebreak printf("\n\n")

#define assert(a, b) printf("%d\n", fstr_equals_C(a,b))

void test_from_C() {
    printf("Test From C: ");
    fstr *str = fstr_from_C("0123456789");
    assert(str, "0123456789");
    free(str);
}

void test_from_format_c() {
    printf("Test From C Format: ");
    fstr *str = fstr_from_format_C("%d %.5f %s", 10, 12.123456789, "abcd");
    assert(str, "10 12.12346 abcd");

    free(str);
}

void test_from_length() {
    printf("Test From Length: ");
    fstr *str = fstr_from_length(10, '.');
    assert(str, "..........");
    free(str);
}

void test_append_c() {
    printf("Test Append C: ");
    fstr *str = fstr_from_C("Hello");
    fstr_append_C(str, " World");
    assert(str, "Hello World");
    free(str);
}

void test_append() {
    printf("Test Append: ");
    fstr *str = fstr_from_C("Hello");
    fstr *add = fstr_from_C(" World!");
    fstr_append(str, add);

    assert(str, "Hello World");

    free(str);
    free(add);
}

void fstr_run_tests() {
    usize i = 0;
    usize iterations = 1;

    for (i = 0; i < iterations; i++) {
        test_from_C();
    }

    for (i = 0; i < iterations; i++) {
        test_from_format_c();
    }

    for (i = 0; i < iterations; i++) {
        test_from_length();
    }

    for (i = 0; i < iterations; i++) {
        test_append_c();
    }

    for (i = 0; i < iterations; i++) {
        test_append();
    }
}
