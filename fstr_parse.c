//
// Created by tobin on 3/18/2024.
//

#include <stdlib.h>
#include <wchar.h>
#include <math.h>
#include "fstr_parse.h"


//Also in fstr.h
//#define USING_WCHAR (sizeof(chr) == sizeof(wchar_t))
//#define USING_CHAR (sizeof(chr) == sizeof(char))

#define i64 int64_t
#define u8 uint8_t

bool is_char(chr c) {
    if (USING_CHAR) {
        return (chr_is_lower(c) || chr_is_upper(c));
    } else {
        return iswalpha(c);
    }
}

bool is_digit(chr c) {
    if (USING_CHAR) {
        return (c >= 48 && c <= 57);
    } else {
        return iswdigit(c);
    }
}

uint8_t lookup_char(chr c) {
    if (USING_CHAR) {
        return c - 48;
    } else {
        return wcstol(&c, NULL, 10);
    }
}

bool is_neg(chr c) {
    if (USING_CHAR) {
        return c == '-';
    } else {
        return c == L'-';
    }
}

usize fstr_get_parse_error() {
    return 0;
}

u8 fstr_try_to_i64(const fstr *str, int64_t *out) {
    usize len = fstr_length(str);

    if (len == 0) {
        return 0;
    }

    signed char sign = 1;
    usize start = 0;

    //Check if the string starts with a negative sign, if so we want to skip this chr
    if (is_neg(str->data[0])) {
        //Set the starting index to one and the sign to -1
        start = 1;
        sign = -1;
    }

    i64 final_val = 0;

    usize index;

    for (index = start; index < len; index++) {

        char c = str->data[index];

        //If its not a digit we want to quit, we also set out just for the sake of getting what number has been made so far
        if (!is_digit(c)) {
            goto FailureReturn;
        }

        uint8_t digit = lookup_char(c);

        //If adding a new digit would cause an overflow, we just set it to the max
        if ((final_val * 10) + digit < 0) {
            final_val = INT64_MAX;
            goto FailureReturn;
        }
        //Increment the final value by the current power (think of digit position) times the current character as a digit
        final_val *= 10;
        final_val += digit;
    }

    //Set the out value
    *out = final_val * sign;
    return 1;

    //An alternate failure path where the value is still assigned but we return 0
    FailureReturn:
    {
        *out = final_val * sign;
        return 0;
    }
}