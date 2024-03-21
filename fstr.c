//
// Created by tobin on 3/8/2024.
//

#include "fstr.h"
#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

#define as_ptr(a) ((usize) (a))

#define u8 uint8_t

/// Derived from GCC implementation, made more readable
/// https://github.com/gcc-mirror/gcc/blob/master/libgcc/memcpy.c
/// \param destination Where the memory is going to be put
/// \param src Where the memory is being read from
/// \param size The size of the data to be copied
void memcpy_internal(void *destination, const void *src, usize size) {
    u8 *destStart = destination;
    const char *readStart = src;

    while (size--) {
        *destStart = *readStart;
        *readStart++;
        *destStart++;
    }
}

/// Internal implementation of memset. Technically derived from GCC, but i wrote this before seeing the implementation
/// \param destination The start of the memory to be changed
/// \param fill The chr to fill the data with
/// \param size The total size of the data
void memset_internal(void *destination, const chr fill, usize size) {
    u8 *dest = destination;
    while (size--) {
        *dest = fill;
        dest++;
    }
}

usize fstr_length(const fstr *str) {
    usize diff = ((as_ptr(str->end) - as_ptr(str->data)) / sizeof(chr));
    return diff;
}

/// Sets the end pointer of the fstr
/// \param str
/// \param newLength
void internal_fstr_set_end(fstr *str, usize newLength) {
    //Set the end to the address last indexed character of the string
    str->end = as_ptr(&str->data[newLength]);
}

/// Custom String Length function
/// \param buf The string to check
/// \return The length of the string, NOT including the null terminator
usize internal_C_string_length(const chr *buf) {
    if (USING_WCHAR) {
        return wcslen(buf);
    } else {
        usize i = 0;
        while (buf[i] != '\0') {
            i++;
        }

        return i;
    }
}

fstr *fstr_copy(const fstr *str) {
    usize len = fstr_length(str);
    fstr *new = fstr_from_length(len, '!');
    memcpy_internal(new->data, str->data, len * sizeof(chr));
    return new;
}

void fstr_replace_chr(fstr *str, const chr from, const chr to) {
    usize len = fstr_length(str);

    usize i;
    for (i = 0; i < len; i++) {
        if (str->data[i] == from) {
            str->data[i] = to;
        }
    }
}


void fstr_remove_at(fstr *str, const usize index, const usize length) {

    usize startLen = fstr_length(str);

    if (startLen == 0) {
        return;
    }

    if (index >= startLen) {
        str->error = STR_ERR_IndexOutOfBounds;
        return;
    }

    //TODO Reimplement in memcpy could be much quicker than iteration on BIG strings
    usize i, subIndex = 0;
    for (i = 0; i < startLen; i++) {
        //if (i != index) {
        if (i < index || i >= (index + length)) {
            str->data[subIndex] = str->data[i];
            subIndex++;
        }
    }

//    memcpy_internal(str->data + (index), str->data + index, (startLen - index) * sizeof(chr));

    internal_fstr_set_end(str, subIndex);
}

void internal_replace_sub(fstr *str, usize len, chr *buf) {
//    for (int i = 0; i <)
}

void internal_remove_buf(fstr *str, const char *removeBuf, const usize removeLen) {
    usize len = fstr_length(str);
    usize secondary = 0;

    //TODO I wonder if theres a way to remove the alloc...
    //I'm too silly rn to be able to figure it out, but TBH we should be able to just shift our 'i' condition back the length of the remove length, and decrease our len
    //We can also do this non iteratively with memcpy and stuff, but I'm not too worried if this is a bit slow
    fstr *copy = fstr_copy(str);

    usize i;
    for (i = 0; i < len; i++) {
        u8 found = 1;

        //Check for the non-existence of the substring removeBuf
        usize c;
        for (c = 0; c < removeLen; c++) {
            //Do an OOB check,    Do char comparison
            if ((i + c) >= len || copy->data[c + i] != removeBuf[c]) {
                found = 0;
                break;
            }
        }

        //Skip over the stuff we don't want to include
        if (found) {
            i += removeLen - 1;
        }
            //Place our characters into our str
        else {
            str->data[secondary] = copy->data[i];
            secondary++;
        }
    }

    fstr_free(copy);

    internal_fstr_set_end(str, secondary);
}


void fstr_remove(const fstr *str, const fstr *buf) {
    internal_remove_buf(str, buf->data, fstr_length(buf));
}

void fstr_remove_C(const fstr *str, const chr *buf) {
    internal_remove_buf(str, buf, internal_C_string_length(buf));
}

void fstr_append_chr(fstr *str, const chr c) {
    usize len = fstr_length(str);
    str->data = realloc(str->data, (len + 1) * sizeof(chr));

    //Return an error if alloc fails
    if (str->data == NULL) {
        str->error = STR_ERR_ReallocFailed;
        return;
    }

    str->data[len] = c;
    internal_fstr_set_end(str, len + 1);
}

void fstr_remove_chr_varargs(fstr *str, int num_chars, ...) {
    va_list args;

    va_start(args, num_chars);

    usize i;
    for (i = 0; i < num_chars; i++) {
        fstr_remove_chr(str, (chr) va_arg(args, int));
    }

    va_end(args);
}

void fstr_remove_chr(fstr *str, const chr from) {
    usize len = fstr_length(str);
    usize secondary = 0;
    usize primary;

    //Iterate our string and place our non-from strings into our same string in order
    for (primary = 0; primary < len; primary++) {
        if (str->data[primary] != from) {
            str->data[secondary] = str->data[primary];
            secondary++;
        }
    }

    str->data = realloc(str->data, secondary * sizeof(chr));

    internal_fstr_set_end(str, secondary);
}


usize fstr_count_chr(const fstr *str, const chr value) {
    usize len = fstr_length(str);

    usize count = 0;
    usize i;
    for (i = 0; i < len; i++) {
        if (str->data[i] == value) {
            count++;
        }
    }
    return count;
}


u8 fstr_index_of_chr(fstr *str, char c, usize *index) {
    usize i;
    usize len = fstr_length(str);

    for (i = 0; i < len; i++) {
        if (str->data[i] == c) {
            *index = i;
            return 1;
        }
    }

    return 0;
}

fstr *fstr_substr(fstr *str, usize start, usize length) {

    usize len = fstr_length(str);
    if (start >= len) {
        str->error = STR_ERR_IndexOutOfBounds;
        return fstr_from_C("");
    }

    //Create our dummy string
    fstr *sub = fstr_from_length(length, '!');

    //Copy the memory over from our start string
    memcpy_internal(sub->data, str->data + start, length * sizeof(chr));

    internal_fstr_set_end(sub, length);

    return sub;
}

void fstr_set_chr(fstr *str, usize index, chr c) {
    usize len = fstr_length(str);
    if (index >= len) {
        str->error = STR_ERR_IndexOutOfBounds;
        return;
    }

    str->data[index] = c;
}

fstr *fstr_from_C(const chr *buf) {

    //Calculate the size of our buffer
    usize bufSize = internal_C_string_length(buf) * sizeof(chr);

    //Malloc our struct
    fstr *str = malloc(sizeof(fstr));

    //Malloc our data
    str->data = malloc(bufSize);

    //Reset error
    str->error = STR_ERR_None;

    //Copy in our buffer to our data
    memcpy_internal(str->data, buf, bufSize);

    //Set the endpoint of our fstr
    str->end = as_ptr(str->data) + as_ptr(bufSize);

    return str;
}


u8 internal_validate_fstr(fstr *str) {
    if (as_ptr(str->data) > str->end) {
        str->error = STR_ERR_INCORRECT_CHAR_POINTER;
        return 0;
    }

    return 1;
}

void fstr_free(fstr *str) {
    if (str != NULL) {
        if (str->data != NULL) {
            free(str->data);
        }
        free(str);
    }
}

void internal_print_chr(const chr *format, const chr print) {
    if (USING_WCHAR) {
        wprintf(format, print);
    } else if (USING_CHAR) {
        printf(format, print);
    }
}

void fstr_print_chrs(const fstr *str) {
    if (USING_CHAR) {
        usize i;
        usize len = fstr_length(str);
        for (i = 0; i < len; i++) {
            internal_print_chr("%c", str->data[i]);
        }
    }
}

void fstr_print_chrs_f(const fstr *str, const chr *format) {
    usize i;
    usize len = fstr_length(str);
    for (i = 0; i < len; i++) {
        internal_print_chr(format, str->data[i]);
    }
}

void fstr_print(const fstr *str) {
    if (USING_CHAR) {
        usize len = fstr_length(str);
        fwrite(str->data, sizeof(chr), len, stdout);
    } else if (USING_WCHAR) {
        wprintf(L"%ls", str->data);
    }
}


void fstr_println(const fstr *str) {
    fstr_print(str);
    printf("\n");
}

void fstr_print_hex(const fstr *str) {
    usize len = fstr_length(str);
    usize i;
    for (i = 0; i < len; i++) {
        printf("0x%x ", str->data[i]);
    }
}

char *fstr_as_C_heap(const fstr *from) {
    //Get the length of our from string
    usize len = fstr_length(from);

    //Allocate our new memory, plus one on length for the null terminator
    char *toStr = calloc(len + 1, sizeof(char));

    //Copy our source data over
    memcpy_internal(toStr, from->data, len * sizeof(chr));

    //Add the null terminator
    toStr[len] = '\0';

    return toStr;
}

void fstr_append(fstr *str, const fstr *buf) {

    if (buf == NULL) {
        str->error = STR_ERR_NullStringArg;
        return;
    }

    //Get / calculate the lengths that will be involved
    usize startLength = fstr_length(str);
    usize bufLength = fstr_length(buf);
    usize newLength = startLength + bufLength;

    if (newLength == 0) {
        return;
    }

    //Reallocate our string data to include room for our new buf
    str->data = realloc(str->data, newLength * sizeof(chr));

    //Return an error if alloc fails
    if (str->data == NULL) {
        str->error = STR_ERR_ReallocFailed;
        return;
    }

    //Copy the data from our buf onto our str data with an offset of our initial size in bytes
    memcpy_internal(str->data + startLength * sizeof(chr), buf->data, bufLength * sizeof(chr));

    //Set the end to the pointer to the last character of our new string
    internal_fstr_set_end(str, newLength);
}

void fstr_append_C(fstr *str, const chr *buf) {

    if (buf == NULL) {
        str->error = STR_ERR_NullStringArg;
        return;
    }

    //Calculate lengths
    usize startLen = fstr_length(str);
    usize bufLen = internal_C_string_length(buf);
    usize newLen = startLen + bufLen;

    //Realloc the string
    str->data = realloc(str->data, newLen * sizeof(chr));

    //Return an error if realloc fails
    if (str->data == NULL) {
        str->error = STR_ERR_ReallocFailed;
        return;
    }

    //Copy the string memory in
    memcpy_internal(str->data + startLen, buf, bufLen * sizeof(chr));

    //Recalculate the end
    internal_fstr_set_end(str, newLen);
}

fstr *fstr_from_length(usize length, const chr fill) {

    //Error check
    if (length <= 0) {
        return fstr_from_C("");
    }

    //Create our string
    fstr *str = fstr_from_C("");

    //Malloc new string data with the correct size
    str->data = malloc(length * sizeof(chr));

    //Set our error to 0
    str->error = STR_ERR_None;

    //Set all the memory to our fill character
    memset_internal(str->data, fill, length * sizeof(chr));

    //Set the end pointer to the last character of our stringF
//    str->end = as_ptr(&str->data[length]);
    internal_fstr_set_end(str, length);

    return str;
}

fstr *fstr_from_format_C(const char *format, ...) {

    //Varargs stuff
    va_list args;
    va_start(args, format);

    //Calculate the size of the buffer
    int size = _vscprintf(format, args);

    //Create the new string, we divide by sizeof chr in case chars are bigger
    //TODO This divide could be wrong
    fstr *str = fstr_from_length(size / sizeof(chr), '!');

    //Write the varargs to the string with the proper format
    vsprintf(str->data, format, args);

    va_end(args);

    return str;
}

void fstr_append_format_C(fstr *str, const char *format, ...) {

    if (format == NULL) {
        str->error = STR_ERR_NullStringArg;
        return;
    }

    va_list args;
    va_start(args, format);

    //Get the sizes
    usize startSize = fstr_length(str) * sizeof(chr);
    usize addSize = _vscprintf(format, args) / sizeof(chr);
    usize finalSize = startSize + addSize;

    //Reallocate the data of the string to fit our finalSize
    str->data = realloc(str->data, finalSize * sizeof(chr));

    //Return an error if alloc fails
    if (str->data == NULL) {
        str->error = STR_ERR_ReallocFailed;
        return;
    }

    //Do our vsprintf to the data, offset by our start size as to write our data to our unneeded stuff
    vsprintf(str->data + startSize, format, args);

    //Set the end of our string
    internal_fstr_set_end(str, finalSize / sizeof(chr));

    va_end(args);
}

chr chr_to_invert(chr a) {
    if (USING_CHAR) {
        if ((a >= 65 && a <= 90) || (a >= 97 && a <= 122)) {
            //This works great for well formed non ASCII extended characters
            return (chr) (a ^ 0b00100000);
        }
    } else if (USING_WCHAR) {
        if (chr_is_lower(a)) {
            return towupper(a);
        } else if (chr_is_upper(a)) {
            return towlower(a);
        }
    }

    return a;
}

chr chr_to_lower(chr a) {
    if (chr_is_upper(a)) {
        return chr_to_invert(a);
    }
    return a;
}

chr chr_to_upper(chr a) {
    if (chr_is_lower(a)) {
        return chr_to_invert(a);
    }

    return a;
}


void fstr_to_lower(fstr *a) {
    usize len = fstr_length(a);
    usize i;
    for (i = 0; i < len; i++) {
        a->data[i] = chr_to_lower(a->data[i]);
    }
}

void fstr_to_upper(fstr *a) {
    usize len = fstr_length(a);
    usize i;
    for (i = 0; i < len; i++) {
        a->data[i] = chr_to_upper(a->data[i]);
    }
}

void fstr_invertcase(fstr *a) {
    usize len = fstr_length(a);
    usize i;
    for (i = 0; i < len; i++) {
        chr c = a->data[i];
        a->data[i] = chr_to_invert(c);
    }
}

u8 fstr_equals(fstr *a, fstr *b) {

    if (b == NULL) {
        a->error = STR_ERR_NullStringArg;
        return 0;
    }

    usize aLen = fstr_length(a);
    usize bLen = fstr_length(b);

    if (aLen != bLen) {
        return 0;
    }

    usize i;
    for (i = 0; i < aLen; i++) {
        if (a->data[i] != b->data[i]) {
            return 0;
        }
    }

    return 1;
}


void fstr_reverse(fstr *str) {
    usize len = fstr_length(str);

    if (len == 0 || len == 1) {
        return;
    }

    //TODO Refactor fstr_reverse. It's fine but not quite clean
    if (len == 2) {
        chr tmp = str->data[0];
        str->data[0] = str->data[1];
        str->data[1] = tmp;
        return;
    }

    usize i = 0;
    usize j = len;

    for (i; i < len; i++) {
        j--;

        chr tmp = str->data[i];
        str->data[i] = str->data[j];
        str->data[j] = tmp;

        //TODO Refactor fstr_reverse. It's fine but not quite clean
        if (j == i) {
            break;
        }
    }
}

void fstr_clear(fstr *str) {
    free(str->data);
    str->data = calloc(1, sizeof(chr));
    internal_fstr_set_end(str, 1);
}

/// The internal string insert function
/// \param str
/// \param add
/// \param index
/// \param addLen
void internal_fstr_insert(fstr *str, const char *add, usize index, usize addLen) {

    usize startLen = fstr_length(str);
    usize finalLen = startLen + addLen;

    if (addLen == 0) {
        return;
    }

    if (index >= finalLen) {
        str->error = STR_ERR_IndexOutOfBounds;
        return;
    }

    str->data = realloc(str->data, finalLen * sizeof(chr));

    //Return an error if realloc fails
    if (str->data == NULL) {
        str->error = STR_ERR_ReallocFailed;
        return;
    }

    //In the future for debugging / rewriting, draw out memory on paper to make it more understandable

    //Get the buffer size of the right data that will be shifted
    usize rightBuffSize = (startLen - index) * sizeof(chr);

    //Weve gotta write into a temporary buffer cuz otherwise we end up reading then rewriting already written data
    chr *tmp = malloc(rightBuffSize);

    //Write the right most data into a temporary buffer
    memcpy_internal(tmp, str->data + index, rightBuffSize);

    //Copy the right most data and place it at its ideal location, leaving some remaining data after index with size of add
    memcpy_internal(str->data + (index + addLen), tmp, rightBuffSize);

    free(tmp);

    //Replace the data after index with the size of add with the actual add data
    memcpy_internal(str->data + index, add, addLen * sizeof(chr));

    internal_fstr_set_end(str, finalLen);
}

void fstr_insert(fstr *str, usize index, const fstr *add) {

    if (add == NULL) {
        str->error = STR_ERR_NullStringArg;
        return;
    }

    internal_fstr_insert(str, add->data, index, fstr_length(add));
}

void fstr_insert_C(fstr *str, usize index, const chr *add) {
    if (add == NULL) {
        str->error = STR_ERR_NullStringArg;
        return;
    }

    internal_fstr_insert(str, add, index, internal_C_string_length(add));
}

void fstr_pad(fstr *str, usize targetLength, chr pad, int8_t side) {

    usize currentLen = fstr_length(str);

    if (targetLength <= currentLen) {
        str->error = STR_ERR_IndexOutOfBounds;
        return;
    }

    usize diff = targetLength - currentLen;

    ///Pad the left side
    if (side < 0) {
        //Pad the left side
        fstr *prePad = fstr_from_length(diff, pad);
        fstr_insert(str, 0, prePad);
        free(prePad);
        return;
    }
    ///Pad both sides
    if (side == 0) {
        //Insert the pad on the left
        fstr *left = fstr_from_length(diff / 2, pad);
        fstr_insert(str, 0, left);

        //Pad the remaining right side. This is technically recursive but only ever a depth of 1
        fstr_pad(str, targetLength, pad, 1);

        //Free the left memory
        free(left);
        return;
    }
    ///Pad the right side
    if (side > 0) {
        //Pad the right side
        fstr *prePad = fstr_from_length(diff, pad);

        usize len = fstr_length(str);
        fstr_insert(str, len, prePad);
        free(prePad);

        return;
    }
}

u8 fstr_succeeded(fstr *str) {
    return str->error == 0;
}