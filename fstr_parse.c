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

u8 fstr_try_to_i64(const fstr *str, int64_t *outValue) {

    //Make a copy of the string
    fstr *cop = fstr_copy(str);

    //Remove newlines, spaces, etc.
    fstr_trim(cop, 0);

    usize len = fstr_length(cop);

    if (len == 0) {
        return 0;
    }

    signed char sign = 1;
    usize start = 0;

    //Check if the string starts with a negative sign, if so we want to skip this chr
    if (is_neg(cop->data[0])) {
        //Set the starting index to one and the sign to -1
        start = 1;
        sign = -1;
    }

    i64 final_val = 0;

    usize index;

    for (index = start; index < len; index++) {

        char c = cop->data[index];

        //If its not a digit we want to quit, we also set outValue just for the sake of getting what number has been made so far
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

    //DefaultReturn
    {
        //Set the outValue value
        *outValue = final_val * sign;
        free(cop);
        return 1;
    }

    //An alternate failure path where the value is still assigned but we return 0
    FailureReturn:
    {
        *outValue = final_val * sign;
        free(cop);
        return 0;
    }
}

int64_t fstr_to_i64(const fstr *str) {
    int64_t value = 0;
    fstr_try_to_i64(str, &value);
    return value;
}


bool fstr_try_to_u64(const fstr *str, uint64_t *outValue) {
    i64 value = 0;
    u8 ret = fstr_try_to_i64(str, &value);
    *outValue = to_u64(value);
    return ret;
}

u64 fstr_to_u64(const fstr *str) {
    u64 val = 0;
    fstr_try_to_u64(str, &val);
    return val;
}
