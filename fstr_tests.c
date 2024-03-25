//
// Created by tobin on 3/20/2024.
//

#include <stdio.h>
#include <malloc.h>
#include "fstr_tests.h"
#include "fstr.h"

#define linebreak printf("\n\n")

void test_from_C() {
    printf("Test From C:\nfstr: ");
    fstr *str = fstr_from_C("0123456789");
    fstr_print(str);
    printf("|\ncstr: 0123456789|");
    free(str);
}

void test_from_format_c() {
    printf("Test From C\nfstr: ");
    fstr *str = fstr_from_format_C("%d %.5f %s", 10, 12.123456789, "abcd");

    fstr_print(str);
    printf("|\ncstr: 10 12.12345 abcd|");

    free(str);
}

void test_from_length() {
    printf("Test From Length\nfstr: ");
    fstr *str = fstr_from_length(10, '.');
    fstr_print(str);
    printf("|\ncstr: ..........|");
    free(str);
}

void test_append_c() {
    printf("Test Append C\nfstr: ");
    fstr *str = fstr_from_C("Hello");
    fstr_append_C(str, " World!");
    fstr_print(str);
    printf("|\ncstr: Hello World!|");

    free(str);
}

void test_append() {
    printf("Test Append\nfstr: ");
    fstr *str = fstr_from_C("Hello");
    fstr *add = fstr_from_C(" World!");
    fstr_append(str, add);
    fstr_print(str);
    printf("|\ncstr: Hello World!|");

    free(str);
    free(add);
}


void fstr_run_tests() {
    test_from_C();

    linebreak;

    test_from_format_c();

    linebreak;

    test_from_length();

    linebreak;

    test_append_c();

    linebreak;

    test_append();

    linebreak;


}
