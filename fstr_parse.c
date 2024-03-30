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
#define u64 uint64_t

#define i32 int32_t
#define u32 uint32_t

#define i16 int16_t
#define u16 uint16_t

#define i8 int8_t
#define u8 uint8_t

#define bool uint8_t

#define FAILURE (fstr_result) {0}

bool is_alpha(chr c) {
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

u64 to_u64(i64 val) {
    if (val < 0) {
        val = -val;
    }

    return (u64) val;
}


fstr_result fstr_to_double(const fstr *str) {
    fstr_result res = fstr_to_i64(str);
    i64 tmp = res.i_val;
    res.f_val = (float) tmp;
    return res;
}

fstr_result fstr_to_i64(const fstr *str) {

    //Make a copy of the string
    fstr *cop = fstr_copy(str);

    //Remove newlines, spaces, etc.
    fstr_trim(cop, 0);

    usize len = fstr_length(cop);

    if (len == 0) {
        return FAILURE;
    }

    i8 sign = 1;
    usize start = 0;

    //Check if the string starts with a negative sign, if so we want to skip this chr
    if (is_neg(cop->data[0])) {
        //Set the starting i_val to one and the sign to -1
        start = 1;
        sign = -1;
    }

    i64 final_val = 0;

    register usize index;

    fstr_result result = {1, 0};

    for (index = start; index < len; index++) {

        char c = cop->data[index];

        //If its not a digit we want to quit, we also set outValue just for the sake of getting what number has been made so far
        if (!is_digit(c)) {
            result.success = 0;
            break;
        }

        uint8_t digit = lookup_char(c);

        //If adding a new digit would cause an overflow, we just set it to the max
        if ((final_val * 10) + digit < 0) {
            final_val = INT64_MAX;
            result.success = 0;
        }
        //Increment the final value by the current power (think of digit position) times the current character as a digit
        final_val *= 10;
        final_val += digit;
    }

    result.i_val = final_val * sign;

    free(cop);
    return result;
}