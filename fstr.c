//
// Created by tobin on 3/8/2024.
//

#include "fstr.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <wchar.h>

#define as_ptr(a) ((PTR_SIZE) (a))

#define u64 uint64_t
#define u8 uint8_t
#define llu unsigned long long


PTR_SIZE fstr_length(const fstr *str) {
    uint64_t diff = (as_ptr(str->end) - as_ptr(str->data)) / sizeof(chr);
    return diff;
}

/// Sets the end pointer of the fstr
/// \param str
/// \param newLength
void internal_fstr_set_end(fstr *str, PTR_SIZE newLength) {
    //Set the end to the address last indexed character of the string
    str->end = as_ptr(&str->data[newLength]);
}

/// Custom String Length function
/// \param buf The string to check
/// \return The length of the string, NOT including the null terminator
llu internal_C_string_length(const chr *buf) {
    if (USING_WCHAR) {
        return wcslen(buf);
    } else {
        PTR_SIZE i = 0;
        while (buf[i] != '\0') {
            i++;
        }

        return i;
    }
}

fstr *fstr_copy(const fstr *str) {
    PTR_SIZE len = fstr_length(str);
    fstr *new = fstr_from_length(len, '!');
    memcpy(new->data, str->data, len * sizeof(chr));
    return new;
}

void fstr_replace_chr(fstr *str, const chr from, const chr to) {
    PTR_SIZE len = fstr_length(str);

    PTR_SIZE i;
    for (i = 0; i < len; i++) {
        if (str->data[i] == from) {
            str->data[i] = to;
        }
    }
}

void internal_remove_buf(const fstr *str, const char *removeBuf, const PTR_SIZE removeLen) {
    PTR_SIZE len = fstr_length(str);
    PTR_SIZE i;
    PTR_SIZE secondary = 0;

    //TODO I wonder if theres a way to remove the alloc...
    //I'm too silly rn to be able to figure it out, but TBH we should be able to just shift our 'i' condition back the length of the remove length, and decrease our len
    //We can also do this non iteratively with memcpy and stuff, but I'm not too worried if this is a bit slow
    fstr *copy = fstr_copy(str);

    for (i = 0; i < len; i++) {
        u8 found = 1;

        //Check for the non-existence of the substring removeBuf
        PTR_SIZE c;
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
    internal_remove_buf(str, buf, strlen(buf));
}


void fstr_append_chr(fstr *str, const chr c) {
    PTR_SIZE len = fstr_length(str);
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

    PTR_SIZE i;
    for (i = 0; i < num_chars; i++) {
        fstr_remove_chr(str, (chr) va_arg(args, int));
    }

    va_end(args);
}

void fstr_remove_chr(fstr *str, const chr from) {
    PTR_SIZE len = fstr_length(str);
    PTR_SIZE secondary = 0;
    PTR_SIZE primary;

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


PTR_SIZE fstr_count_chr(const fstr *str, const chr value) {
    PTR_SIZE len = fstr_length(str);

    PTR_SIZE count = 0;
    PTR_SIZE i;
    for (i = 0; i < len; i++) {
        if (str->data[i] == value) {
            count++;
        }
    }
    return count;
}

fstr *fstr_substr(fstr *str, int start, PTR_SIZE length) {
    fstr *sub = fstr_from_length(length, '!');

    memcpy(sub->data, str->data + start, length * sizeof(chr));

    internal_fstr_set_end(sub, length);

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

fstr *fstr_from_C(const chr *buf) {

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
    free(str->data);
    free(str);
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
        PTR_SIZE i;
        u64 len = fstr_length(str);
        for (i = 0; i < len; i++) {
            internal_print_chr("%c", str->data[i]);
        }
    }
}


void fstr_print_chrs_f(const fstr *str, const chr *format) {
    PTR_SIZE i;
    u64 len = fstr_length(str);
    for (i = 0; i < len; i++) {
        internal_print_chr(format, str->data[i]);
    }
}

void fstr_print(const fstr *str) {
    if (USING_CHAR) {
        u64 len = fstr_length(str);
        write(STDOUT_FILENO, str->data, len);
    } else if (USING_WCHAR) {
        wprintf(L"%ls", str->data);
    }
}


void fstr_println(const fstr *str) {
    fstr_print(str);
    printf("\n");
}

void fstr_print_hex(const fstr *str) {
    u64 len = fstr_length(str);
    PTR_SIZE i;
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
    memcpy(str->data + startLength * sizeof(chr), buf->data, bufLength * sizeof(chr));

    //Set the end to the pointer to the last character of our new string
    internal_fstr_set_end(str, newLength);
}

void fstr_append_C(fstr *str, const chr *buf) {

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
    memcpy(str->data + startLen, buf, bufLen * sizeof(chr));

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
    PTR_SIZE len = fstr_length(a);
    PTR_SIZE i;
    for (i = 0; i < len; i++) {
        a->data[i] = chr_to_lower(a->data[i]);
    }
}

void fstr_to_upper(fstr *a) {
    PTR_SIZE len = fstr_length(a);
    PTR_SIZE i;
    for (i = 0; i < len; i++) {
        a->data[i] = chr_to_upper(a->data[i]);
    }
}

void fstr_invertcase(fstr *a) {
    PTR_SIZE len = fstr_length(a);
    PTR_SIZE i;
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

    u64 aLen = fstr_length(a);
    u64 bLen = fstr_length(b);

    if (aLen != bLen) {
        return 0;
    }

    PTR_SIZE i;
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
    memcpy(str->data + (index + addLen), str->data + index, (startLen - index) * sizeof(chr));
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

    internal_fstr_insert(str, add->data, index, as_ptr(fstr_length(add)));
}

void fstr_insert_c(fstr *str, const chr *add, uint64_t index) {
    if (add == NULL) {
        str->error = STR_ERR_NullStringArg;
        return;
    }

    internal_fstr_insert(str, add, index, as_ptr(internal_C_string_length(add)));
}

void fstr_pad(fstr *str, uint64_t targetLength, chr pad, int8_t side) {

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

u8 fstr_succeeded(fstr *str) {
    return str->error == 0;
}