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
#define FAILURE (fstr_result) {0}


//Internal function prototypes
#pragma region PROTOTYPES

void internal_fstr_insert(fstr *str, uintptr_t index, const char *add, uintptr_t addLen);

void internal_remove_buf(fstr *str, const char *removeBuf, const usize removeLen);

void memcpy_internal(void *destination, const void *src, usize size);


#pragma endregion PROTOTYPES

typedef struct {
    uint64_t _0, _1, _2, _3, _4, _5, _6, _7;
} MEM_BLOCK_64B;

typedef struct {
    uint64_t _0, _1, _2, _3;
} MEM_BLOCK_32B;

/// Custom memcpy implementation using chunking along with basic iterative assignment
/// \param destination Where the memory is going to be put
/// \param src Where the memory is being read from
/// \param size The size of the data to be copied
void memcpy_internal(void *destination, const void *src, usize size) {

    u8 *dest = destination;
    const char *read = src;

    //Copy 64 byte size chunks
    {
        usize chunkSize = sizeof(MEM_BLOCK_64B);

        while (size >= chunkSize) {
            *((MEM_BLOCK_64B *) dest) = *((MEM_BLOCK_64B *) read);
            size -= chunkSize;
            dest += chunkSize;
            read += chunkSize;
        }
    }

    //Copy 8 byte sized chunks
    {
        usize chunkSize = sizeof(uint64_t);

        while (size >= chunkSize) {
            *((uint64_t *) dest) = *((uint64_t *) read);
            size -= chunkSize;
            dest += chunkSize;
            read += chunkSize;
        }
    }

    //Copy singular bytes
    while (size--) {
        *dest = *read;
        *read++;
        *dest++;
    }
}

u8 memeq_internal(const void *a, const void *b, uintptr_t size) {
    usize counter = 0;

    const u8 *aa = a;
    const u8 *bb = b;

    while (counter < size) {
        if (aa[counter] != bb[counter]) {
            return 0;
        }
        counter++;
    }

    return 1;
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


void fstr_remove_at(fstr *str, const usize index, usize length) {

    usize startLen = fstr_length(str);
    usize startPtr = as_ptr(str->data);

    if (startLen == 0) {
        return;
    }

    if (index >= startLen) {
        str->error = STR_ERR_IndexOutOfBounds;
        return;
    }

    while (index + length > startLen) {
        length--;

        if (length == 0) {
            str->error = STR_ERR_IndexOutOfBounds;
            return;
        }
    }

    usize i, k = 0;
    for (i = 0; i < startLen; i++) {
        if (i < index || i >= (index + length)) {
            str->data[k] = str->data[i];
            k++;
        }
    }

    str->data = realloc(str->data, k * sizeof(chr));
    internal_fstr_set_end(str, k);
}

/// Returns the index of a substring within the string. Returns to fstr_result.u_val
/// \param str The string to text.
/// \param buf The buffer to check for
/// \param len The length of the buffer
/// \return The result with the index in u_val
fstr_result internal_index_of_sub(const fstr *str, const char *buf, const uintptr_t len) {
    usize strlen = fstr_length(str);

    if (len > strlen) {
        return FAILURE;
    }

    usize i;
    for (i = 0; i < strlen; i++) {
        u8 success = 1;
        usize k;

        //Checking if substring off this char is legit
        for (k = 0; k < len; k++) {
            //If substring u_val is out of bounds
            if (i + k >= strlen) {
                success = 0;
                break;
            }

            //If the data doesnt match, break the loop
            if (str->data[i + k] != buf[k]) {
                success = 0;
                break;
            }
        }

        if (success) {
            fstr_result res = (fstr_result) {1};
            res.u_val = i;
            return res;
        }
    }

    return FAILURE;
}


fstr_result fstr_index_of_C(const fstr *str, char *sub) {
    return internal_index_of_sub(str, sub, internal_C_string_length(sub));
}


fstr_result fstr_index_of(const fstr *str, const fstr *sub) {
    return internal_index_of_sub(str, sub->data, fstr_length(sub));
}


/// Takes a slice of the internal string with the data pointing to the original sliced string. <br/>
/// THIS MEANS THAT ANY CHANGES TO THIS STRINGS DATA WILL AFFECT THE SLICED STRING
/// \param str The string to slice
/// \param start The starting index
/// \param length The length of the slice to take
/// \return The slice which contains a pointer to the strs data
fstr internal_slice(fstr *str, uintptr_t start, uintptr_t length) {
    fstr res = (fstr) {0};
    res.data = str->data + start;
    res.end = (usize) (str->data + start + length);
    return res;
}

/// Replaces any instances of oldBuf with newBuf
/// \param str The string to have contents replaced
/// \param oldBuf The old buffer to look for
/// \param oldLen The length of the old buffer
/// \param newBuf The new buffer to replace the old
/// \param newLen The length of the new buffer
void internal_replace_sub(fstr *str,
                          const chr *oldBuf, const usize oldLen,
                          const chr *newBuf, const usize newLen) {

    if (newLen == 0) {
        internal_remove_buf(str, oldBuf, oldLen);
        return;
    }

    //Make a copy of our string, the data points to the same location as our
    //actual str data location
    fstr slice = *str;

    usize offset = 0;

    usize removed = 0;

    //Iterate our string
    u8 contains = 1;
    while (contains) {

        //Get the index of our substring (if it exists)
        fstr_result res = internal_index_of_sub(&slice, oldBuf, oldLen);
        contains = res.success;
        usize index = res.u_val;

        //If we contain our substring
        if (contains) {

            //Get the distance between our slice start and our string start as the offset
            offset = (as_ptr(slice.data) - as_ptr(str->data));

            //Remove the substring and replace it
            fstr_remove_at(str, index + offset, oldLen);
            internal_fstr_insert(str, index + offset, newBuf, newLen);

            removed++;

            //Update the end of our slice
            slice.data = str->data + (removed * newLen);
            slice.end = str->end;

            //Do a check to see if we are out of string bounds
            if (slice.data > slice.end) {
                contains = 0;
            }
        }
    }
}

void fstr_replace_C(const fstr *str, const chr *oldBuf, const chr *newBuf) {
    internal_replace_sub(str,
                         oldBuf, internal_C_string_length(oldBuf),
                         newBuf, internal_C_string_length(newBuf));
}

void fstr_replace(const fstr *str, const fstr *oldBuf, const fstr *newBuf) {
    internal_replace_sub(str,
                         oldBuf->data, fstr_length(oldBuf),
                         newBuf->data, fstr_length(newBuf));
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

void fstr_remove_chr_varargs(fstr *str, u8 num_chars, ...) {
    va_list args;
    va_start(args, num_chars);

    usize i;
    for (i = 0; i < num_chars; i++) {
        fstr_remove_chr(str, (chr) va_arg(args, int));
    }

    va_end(args);
}

void fstr_remove_chr(fstr *str, const chr c) {
    usize len = fstr_length(str);
    usize secondary = 0;
    usize primary;

    //Iterate our string and place our non-c strings into our same string in order
    for (primary = 0; primary < len; primary++) {
        if (str->data[primary] != c) {
            str->data[secondary] = str->data[primary];
            secondary++;
        }
    }

    str->data = realloc(str->data, secondary * sizeof(chr));

    internal_fstr_set_end(str, secondary);
}


usize fstr_count_internal(const fstr *str, const chr *sub, usize subLength) {
    usize count = 0;

    usize i = 0;
    for (i = 0; i < fstr_length(str); i++) {
        if (str->data[i] == sub[0]) {
            if (memeq_internal(str->data + i, sub, subLength)) {
                count++;
            }
        }
    }

    return count;
}


usize fstr_count_C(const fstr *str, const char *sub) {
    return fstr_count_internal(str, sub, internal_C_string_length(sub));
}


usize fstr_count(const fstr *str, const fstr *sub) {
    return fstr_count_internal(str, sub->data, fstr_length(sub));
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


fstr_result fstr_index_of_chr(fstr *str, char c) {
    usize i;
    usize len = fstr_length(str);

    for (i = 0; i < len; i++) {
        if (str->data[i] == c) {
            return (fstr_result) {1, i};
        }
    }

    return FAILURE;
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

void internal_print_chr(const chr *format, const chr printChr) {
    if (USING_WCHAR) {
        wprintf(format, printChr);
    } else if (USING_CHAR) {
        printf(format, printChr);
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
    if (str == NULL) {
        return;
    }

    if (USING_CHAR) {
        usize len = fstr_length(str);
        fwrite(str->data, sizeof(chr), len, stdout);
    } else if (USING_WCHAR) {
        wprintf(L"%ls", str->data);
    }
}


void fstr_println(const fstr *str) {
    fstr_print(str);

    if (USING_CHAR) {
        fwrite("\n", sizeof(chr), 1, stdout);
    } else if (USING_WCHAR) {
        wprintf(L"\n");
    }
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
    usize size = _vscprintf(format, args);

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

chr internal_chr_is_trim(chr c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f');
}

void fstr_trim(fstr *str, int8_t side) {

    //TODO There must be a better way to do two for loops going in counter directions with same/similar functionality

    //If we are trimming the left side
    if (side <= 0) {

        usize startTrim = 0;
        usize i;

        //Go through the string, keep count of any trim characters, if theres a non trim character then we exit and cut that off
        for (i = 0; i < fstr_length(str); i++) {
            chr c = str->data[i];
            if (internal_chr_is_trim(c)) {
                startTrim++;
            } else {
                break;
            }
        }

        fstr_remove_at(str, 0, startTrim);
    }
    if (side >= 0) {
        usize endTrim = 0;
        usize i;
        //See (side <= 0)
        for (i = fstr_length(str) - 1; i >= 0; i--) {
            chr c = str->data[i];
            if (internal_chr_is_trim(c)) {
                endTrim++;
            } else {
                break;
            }
        }

        fstr_remove_at(str, fstr_length(str) - endTrim, endTrim);
    }
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
    internal_fstr_set_end(str, 0);
}

/// The internal string insert function
/// \param str
/// \param add
/// \param index
/// \param addLen
void internal_fstr_insert(fstr *str, uintptr_t index, const char *add, uintptr_t addLen) {

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

    //Copy the right most data and place it at its ideal location, leaving some remaining data after u_val with size of add
    memcpy_internal(str->data + (index + addLen), tmp, rightBuffSize);

    free(tmp);

    //Replace the data after u_val with the size of add with the actual add data
    memcpy_internal(str->data + index, add, addLen * sizeof(chr));

    internal_fstr_set_end(str, finalLen);
}

void fstr_insert(fstr *str, usize index, const fstr *add) {

    if (add == NULL) {
        str->error = STR_ERR_NullStringArg;
        return;
    }

    internal_fstr_insert(str, index, add->data, fstr_length(add));
}

void fstr_insert_C(fstr *str, usize index, const chr *add) {
    if (add == NULL) {
        str->error = STR_ERR_NullStringArg;
        return;
    }

    internal_fstr_insert(str, index, add, internal_C_string_length(add));
}

fstr **fstr_split(fstr *str) {

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