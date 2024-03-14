//
// Created by tobin on 3/8/2024.
//

#include "fstr.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#define asptr(a) ((PTR_SIZE) (a))

#define u64 uint64_t
#define llu unsigned long long

/// Sets the end pointer of the fstr
/// \param str
/// \param newLength
void internal_fstr_set_end(fstr *str, PTR_SIZE newLength) {
    str->end = asptr(&str->data[newLength]);
}

/// Custom String Length function
/// \param buf The string to check
/// \return The length of the string, NOT including the null terminator
llu internal_C_string_length(const char *buf) {
    int i = 0;
    while (buf[i] != '\0') {
        i++;
    }

    return i;
}


//void fstr_copy(fstr *destination, fstr *source) {
//    var slen = fstr_length(source);
//}

fstr *fstr_substrlen(fstr *str, int start, int length) {
    fstr *sub = fstr_from_length(length, '!');

    memcpy(sub->data, str->data + start * sizeof(chr), length * sizeof(chr));

    internal_fstr_set_end(sub, length * sizeof(chr));

    return sub;
}

void fstr_replace_chr_at(fstr *str, uint64_t index, chr c) {
    uint64_t len = fstr_length(str);
    if (index >= len) {
        str->error = STR_ERR_IndexOutOfBounds;
        return;
    }

    str->data[index] = c;
}


fstr *fstr_from_wc(const wchar_t *buf) {
    llu bufSize = wcslen(buf) * sizeof(wchar_t);
    fstr *str = malloc(sizeof(fstr));
    str->data = malloc(bufSize);
    str->error = STR_ERR_None;
    memcpy(str->data, buf, bufSize);
    internal_fstr_set_end(str, bufSize);
}

fstr *fstr_from_C(const char *buf) {

    //Calculate the size of our buffer
    llu bufSize = internal_C_string_length(buf) * sizeof(chr);

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
    uint64_t diff = asptr(str->end) - asptr(str->data);
    return diff;
}

uint8_t internal_validate_fstr(fstr *str) {
    if (asptr(str->data) > str->end) {
        str->error = STR_ERR_INCORRECT_CHAR_POINTER;
        return 0;
    }

    return 1;
}

void fstr_free(fstr *str) {
    free(str->data);
    free(str);
}

void fstr_print_slow(const fstr *str) {
    int i;
    u64 len = fstr_length(str);
    for (i = 0; i < len; i++) {
        printf("%c", str->data[i]);
    }
}


void fstr_print_slow_f(const fstr *str, const chr *format) {
    int i;
    u64 len = fstr_length(str);
    for (i = 0; i < len; i++) {
        if (sizeof(chr) == sizeof(wchar_t)) {
            wprintf(format, str->data[i]);
        } else {
            printf(format, str->data[i]);
        }
    }
}

void fstr_print(const fstr *str) {
    u64 len = fstr_length(str);
    write(STDOUT_FILENO, str->data, len);
}


void fstr_print_hex(const fstr *str) {
    u64 len = fstr_length(str);
    int i;
    for (i = 0; i < len; i++) {
        printf("0x%x ", str->data[i]);
    }
}

char *fstr_as_C_heap(const fstr *from) {
    //Get the length of our from string
    u64 len = fstr_length(from);

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
    u64 startLength = fstr_length(str);
    u64 bufLength = fstr_length(buf);
    u64 newLength = startLength + bufLength;

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
    u64 startLen = fstr_length(str);
    llu bufLen = internal_C_string_length(buf);
    llu newLen = startLen + bufLen;

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

fstr *fstr_from_length(uint64_t length, const chr fill) {

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
    memset(str->data, fill, length * sizeof(chr));

    //Set the end pointer to the last character of our stringF
//    str->end = asptr(&str->data[length]);
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
    llu startSize = fstr_length(str) * sizeof(chr);
    llu addSize = _vscprintf(format, args) / sizeof(chr);
    llu finalSize = startSize + addSize;

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

    u64 len = fstr_length(str);

    int i;
    for (i = 0; i < len; i++) {
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

    u64 aLen = fstr_length(a);
    u64 bLen = fstr_length(b);

    if (aLen != bLen) {
        return 0;
    }

    int i;
    for (i = 0; i < aLen; i++) {
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

    u64 startLen = fstr_length(str);
    u64 finalLen = startLen + addLen;

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

    u64 currentLen = fstr_length(str);

    if (targetLength <= currentLen) {
        str->error = STR_ERR_IndexOutOfBounds;
        return;
    }

    u64 diff = targetLength - currentLen;

    ///Pad the left side
    if (side < 0) {
        //Pad the left side
        fstr *prePad = fstr_from_length(diff, pad);
        fstr_insert(str, prePad, 0);
        free(prePad);
        return;
    }
    ///Pad both sides
    if (side == 0) {
        //Insert the pad on the left
        fstr *left = fstr_from_length(diff / 2, pad);
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
        fstr *prePad = fstr_from_length(diff, pad);

        u64 len = fstr_length(str);
        fstr_insert(str, prePad, len);
        free(prePad);

        return;
    }
}

uint8_t fstr_succeeded(fstr *str) {
    return str->error == 0;
}