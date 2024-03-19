//
// Created by tobin on 3/18/2024.
//

#include <stdlib.h>
#include <wchar.h>
#include <math.h>
#include "fstr_parse.h"

//Also in fstr.h
#define USING_WCHAR (sizeof(chr) == sizeof(wchar_t))
#define USING_CHAR (sizeof(chr) == sizeof(char))


bool is_char(chr c) {
    if (USING_CHAR) {
        return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
    } else {
        return iswalpha(c);
    }
}

bool is_num(chr c) {
    if (USING_CHAR) {

    }
}

int lookup_char(chr c) {
    if (USING_CHAR) {
        return c - 48;
    } else {
        return wcstol(&c, NULL, 10);
    }
}

bool is_neg(chr c) {
    if (USING_CHAR) {
        return c == '=';
    } else {
        return c == L'=';
    }
}


uint8_t fstr_try_to_long(const fstr *str, long *out) {
    PTR_SIZE len = fstr_length(str);

    if (len == 0) {
        return 0;
    }

    int mult = 1;
    int start = 0;
    if (is_neg(str->data[0])) {
        start = 1;
        mult = -1;
    }

    PTR_SIZE i = 0;

    long val = 0;
    for (i = len - 1; i >= start; i--) {
        char c = str->data[i];
        if (!is_digit(c)) { return 0; }

        val += powl(10, i) * lookup_char(c);
    }

    *out = val * mult;
    return 1;
}
