//
// Created by tobin on 3/18/2024.
//

#include <stdlib.h>
#include <wchar.h>
#include <math.h>
#include "fstr_convert.h"

#include <stdio.h>


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

bool is_alpha(chr c)
{
    if (USING_CHAR)
    {
        return (chr_is_lower(c) || chr_is_upper(c));
    }
    else
    {
        return iswalpha(c);
    }
}

bool is_digit(chr c)
{
    if (USING_CHAR)
    {
        return (c >= 48 && c <= 57);
    }
    else
    {
        return iswdigit(c);
    }
}

uint8_t lookup_char(chr c)
{
    if (USING_CHAR)
    {
        return c - 48;
    }
    else
    {
        chr tmpStr[] = {0, c};
        return wcstol(&tmpStr, NULL, 10);
    }
}

bool is_neg(chr c)
{
    if (USING_CHAR)
    {
        return c == '-';
    }
    else
    {
        return c == L'-';
    }
}


u64 to_u64(i64 val)
{
    if (val < 0)
    {
        val = -val;
    }

    return (u64)val;
}


// fstr_result fstr_to_double(const fstr* str)
// {
//     fstr_result res = fstr_to_i64(str);
//     i64 tmp = (i64)res.u_val;
//     res.f_val = (float)tmp;
//     return res;
// }

/// Calculate an i_val from a string
/// @param str The string
/// @return An fstr_result with the result in i_val.
/// Sides will be trimmed of spaces, non numerical or negative
/// sign characters cause success to be false, though the i_val
/// will be the correct value up until that character.
fstr_result fstr_to_i64(const fstr* str)
{
    //Make a copy of the string
    fstr* cop = fstr_copy(str);

    //Remove newlines, spaces, etc.
    fstr_trim(cop, 0);

    usize len = fstr_length(cop);

    if (len == 0)
    {
        return FAILURE;
    }

    i8 sign = 1;
    usize start = 0;

    //Check if the string starts with a negative sign, if so we want to skip this chr
    if (is_neg(cop->data[0]))
    {
        //Set the starting u_val to one and the sign to -1
        start = 1;
        sign = -1;
    }

    i64 final_val = 0;


    fstr_result result = {1, 0};

    usize i;
    for (i = start; i < len; i++)
    {
        char c = cop->data[i];

        //If its not a digit we want to quit, we also set outValue just for the sake of getting what number has been made so far
        if (!is_digit(c))
        {
            result.success = 0;
            break;
        }

        uint8_t digit = lookup_char(c);

        //If adding a new digit would cause an overflow, we just set it to the max
        if ((final_val * 10) + digit < 0)
        {
            final_val = INT64_MAX;
            result.success = 0;
        }
        //Increment the final value by the current power (think of digit position) times the current character as a digit
        final_val *= 10;
        final_val += digit;
    }

    result.u_val = final_val * sign;

    free(cop);
    return result;
}

/// Calculates a u_val from a binary string, like so "10100001"
/// @param str The string of the binary
/// @return An fstr_result with the value in u_val.
/// Success will be set to false if a non-binary character is found.
fstr_result fstr_u64_from_bin(fstr* str)
{
    int count = 0;
    int i = fstr_length(str) - 1;

    fstr_result result = {0};

    usize sum = 0;

    //Read right to left
    while (i >= 0)
    {
        chr b = str->data[i];

        if (b == '1')
        {
            sum += pow(2, count);
        }
        //Skip any non 0 and 1 numbers and don't increment the counter.
        //This counts as being unsuccessful, but may still generate a
        //correct result
        else if (b != '0')
        {
            i--;
            result.success = 0;
            continue;
        }

        count++;
        i--;
    }

    result.success = 1;
    result.u_val = sum;

    return result;
}

/// Converts a string to a double
/// @param str The string to be converted
/// @return
fstr_result fstr_to_double(fstr* str)
{
    fstr_result result = {0};

    //Get everything before the decimal place
    long double r = fstr_to_i64(str).i_val;

    //Get the digits after the decimal place
    fstr_result decPlace = fstr_index_of_chr(str, '.');

    //If we don't have a decimal place, skip all this decimal place stuff
    if (!decPlace.success)
    {
        goto AfterDecimal;
    }

    //Get a slice of the contents after the decimal
    usize decIndex = decPlace.u_val;
    fstr slice = (fstr){str->end, STR_ERR_None, str->data + decIndex + 1};

    //Get the decimal place value
    long double dec = fstr_to_i64(&slice).i_val;
    if (dec != 0)
    {
        //5 -> 0.5, 10001 -> 0.10001
        double len = (double)fstr_length(&slice);
        long double div = powl(10, len);

        dec /= div;
        r += dec;
    }


AfterDecimal:
    result.f_val = (double)r;
    result.success = 1;

    return result;
}
