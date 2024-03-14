//
// Created by tobin on 3/8/2024.
//

#include "fstr.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#define var __auto_type
#define asptr(a) ((PTR_SIZE) (a))

/// Sets the end pointer of the fstr
/// \param str
/// \param newLength
void internal_fstr_set_end(fstr *str, PTR_SIZE newLength) {
    str->end = asptr(&str->data[newLength]);
}

/// Custom String Length function
/// \param buf The string to check
/// \return The length of the string, NOT including the null terminator
unsigned long long internal_C_string_length(const char *buf) {
    int i = 0;
    while (buf[i] != '\0') {
        i++;
    }

    return i;
}


//void fstr_copy(fstr *destination, fstr *source) {
//    var slen = fstr_length(source);
//}

void fstr_substr(fstr *str, int min, int max) {

}

void fstr_replace_chr_at(fstr *str, uint64_t index, char c) {
    var len = fstr_length(str);
    if (index >= len) {
        str->error = STR_ERR_IndexOutOfBounds;
        return;
    }

    str->data[index] = c;
}

fstr *fstr_from_C(const char *buf) {

    //Calculate the size of our buffer
    var bufSize = internal_C_string_length(buf) * sizeof(chr);

    //Malloc our struct
    fstr *str = malloc(sizeof(fstr));

    //Malloc our data
    str->data = malloc(bufSize);

    //Reset error
    str->error = STR_ERR_None;

    //Copy in our buffer to our data
    memcpy(str->data, buf, bufSize);

    //Set the endpoint of our fstr
    str->end = asptr(str->data) + asptr(bufSize);

    return str;
}

PTR_SIZE fstr_length(const fstr *str) {

    var diff = asptr(str->end) - asptr(str->data);
    return diff;
}

uint8_t internal_validate_fstr(fstr *str) {
    if (asptr(str->data) > str->end) {
        str->error = STR_ERR_INCORRECT_CHAR_POINTER;
    }
}

void fstr_free(fstr *str) {
    free(str->data);
    free(str);
}

void fstr_print_slow(const fstr *str) {
    for (int i = 0; i < fstr_length(str); i++) {
        printf("%c", str->data[i]);
    }
}

void fstr_print(const fstr *str) {
    var len = fstr_length(str);
    write(STDOUT_FILENO, str->data, len);
}

char *fstr_as_C_heap(const fstr *from) {
    //Get the length of our from string
    var len = fstr_length(from);

    //Allocate our new memory, plus one on length for the null terminator
    char *toStr = calloc(len + 1, sizeof(char));

    //Copy our source data over
    memcpy(toStr, from->data, len * sizeof(chr));

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
    var startLength = fstr_length(str);
    var bufLength = fstr_length(buf);
    var newLength = startLength + bufLength;

    //Reallocate our string data to include room for our new buf
    str->data = realloc(str->data, newLength * sizeof(chr));

    //Return an error if alloc fails
    if (str->data == NULL) {
        str->error = STR_ERR_ReallocFailed;
        return;
    }

    //Copy the data from our buf onto our str data with an offset of our initial size in bytes
    memcpy(str->data + startLength * sizeof(chr), buf->data, bufLength * sizeof(chr));

    //Set the end to the pointer to the last character of our new string
    internal_fstr_set_end(str, newLength);
}

void fstr_append_C(fstr *str, const char *buf) {

    if (buf == NULL) {
        str->error = STR_ERR_NullStringArg;
        return;
    }

    //Calculate lengths
    var startLen = fstr_length(str);
    var bufLen = internal_C_string_length(buf);
    var newLen = startLen + bufLen;

    //Realloc the string
    str->data = realloc(str->data, newLen * sizeof(chr));

    //Return an error if realloc fails
    if (str->data == NULL) {
        str->error = STR_ERR_ReallocFailed;
        return;
    }

    //Copy the string memory in
    memcpy(str->data + startLen * sizeof(chr), buf, bufLen * sizeof(chr));

    //Recalculate the end
    internal_fstr_set_end(str, newLen);
}

fstr *fstr_from_length(uint64_t length, const char fill) {

    //Error check
    if (length <= 0) {
        return fstr_from_C("");
    }

    //Create our string
    var str = fstr_from_C("");

    //Malloc new string data with the correct size
    str->data = malloc(length * sizeof(chr));

    //Set our error to 0
    str->error = STR_ERR_None;

    //Set all the memory to our fill character
    memset(str->data, fill, length * sizeof(chr));

    //Set the end pointer to the last character of our stringF
//    str->end = asptr(&str->data[length]);
    internal_fstr_set_end(str, length);
}

fstr *fstr_from_format_C(const char *format, ...) {

    //Varargs stuff
    va_list args;
    va_start(args, format);

    //Calculate the size of the buffer
    int size = _vscprintf(format, args);

    //Create the new string, we divide by sizeof chr in case chars are bigger
    //TODO This divide could be wrong
    var str = fstr_from_length(size / sizeof(chr), '!');

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
    var startSize = fstr_length(str) * sizeof(chr);
    var addSize = _vscprintf(format, args) / sizeof(chr);
    var finalSize = startSize + addSize;

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

void fstr_replace_chr(fstr *str, chr from, chr to) {

    var len = fstr_length(str);

    for (int i = 0; i < len; i++) {
        if (str->data[i] == from) {
            str->data[i] = to;
        }
    }
}

uint8_t fstr_equals(fstr *a, fstr *b) {

    if (b == NULL) {
        a->error = STR_ERR_NullStringArg;
        return 0;
    }

    var aLen = fstr_length(a);
    var bLen = fstr_length(b);

    if (aLen != bLen) {
        return 0;
    }

    for (int i = 0; i < aLen; i++) {
        if (a->data[i] != b->data[i]) {
            return 0;
        }
    }

    return 1;
}

/// The internal string insert function
/// \param str
/// \param add
/// \param index
/// \param addLen
void internal_fstr_insert(fstr *str, const char *add, PTR_SIZE index, PTR_SIZE addLen) {

    var startLen = fstr_length(str);
    var finalLen = startLen + addLen;

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


    //[A][B][C][D][E][F] <-(2) [Z][X][Y]

    //Shift the first part of the buffer over

    //[A][B][C][D][E][F][-][-][-]
    memcpy(str->data + (index + addLen) * sizeof(chr), str->data + index, (startLen - index) * sizeof(chr));
    //[A][B][-][-][-][C][D][E][F]

    //[A][B][-][-][-][C][D][E][F]
    memcpy(str->data + index, add, addLen * sizeof(chr));
    //[A][B][Z][X][Y][C][D][E][F]

    internal_fstr_set_end(str, finalLen);
}

void fstr_insert(fstr *str, const fstr *add, uint64_t index) {

    if (add == NULL) {
        str->error = STR_ERR_NullStringArg;
        return;
    }

    internal_fstr_insert(str, add->data, index, asptr(fstr_length(add)));
}

void fstr_insert_c(fstr *str, const char *add, uint64_t index) {
    if (add == NULL) {
        str->error = STR_ERR_NullStringArg;
        return;
    }

    internal_fstr_insert(str, add, index, asptr(internal_C_string_length(add)));
}

void fstr_pad(fstr *str, uint64_t targetLength, char pad, int8_t side) {

    var currentLen = fstr_length(str);

    if (targetLength <= currentLen) {
        str->error = STR_ERR_IndexOutOfBounds;
        return;
    }

    var diff = targetLength - currentLen;

    ///Pad the left side
    if (side < 0) {
        //Pad the left side
        var prePad = fstr_from_length(diff, pad);
        fstr_insert(str, prePad, 0);
        free(prePad);
        return;
    }
    ///Pad both sides
    if (side == 0) {
        //Insert the pad on the left
        var left = fstr_from_length(diff / 2, pad);
        fstr_insert(str, left, 0);

        //Pad the remaining right side
        fstr_pad(str, targetLength, pad, 1);

        //Free the left memory
        free(left);
        return;
    }
    ///Pad the right side
    if (side > 0) {
        //Pad the right side
        var prePad = fstr_from_length(diff, pad);

        var len = fstr_length(str);
        fstr_insert(str, prePad, len);
        free(prePad);

        return;
    }
}

uint8_t fstr_succeeded(fstr *str) {
    return str->error == 0;
}