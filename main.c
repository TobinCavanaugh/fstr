#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define var __auto_type
#define PTR_SIZE uint64_t
#define chr char

#define asptr(a) ((PTR_SIZE) a)

//TODO MEMORY ARENA MACROS

typedef struct {
    //The address of the last character in our string, inclusive.
    PTR_SIZE end;

    //The starting pointer of our string data
    chr *data;
} fstr;

/// Custom String Length function to avoid reliance on string.h
/// \param buf The string to check
/// \return The length of the string, NOT including the null terminator
unsigned long long string_length(const char *buf) {
    int i = 0;
    while (buf[i] != '\0') {
        i++;
    }

    return i;
}

/// Creates an fstr from a pre-existing string
/// \param buf
/// \return
fstr *fstr_from_C(const char *buf) {

    //Calculate the size of our buffer
    var bufSize = string_length(buf) * sizeof(chr);

    //Malloc our struct
    fstr *str = malloc(sizeof(fstr));

    //Malloc our data
    str->data = malloc(bufSize);

    //Copy in our buffer to our data
    memcpy(str->data, buf, bufSize);

    //Set the endpoint of our fstr
    str->end = asptr(str->data) + asptr(bufSize);

    return str;
}

/// Returns the length of the string
/// \param str The corresponding string
/// \return The length of the string. Ex: ":)" returns 2
PTR_SIZE fstr_length(const fstr *str) {
    var diff = asptr(str->end) - asptr(str->data);
    return diff;
}

/// Frees the fstr and its data
/// \param str The string to free
void fstr_free(fstr *str) {
    free(str->data);
    free(str);
}

void fstr_slowprint(const fstr *str) {
    for (int i = 0; i < fstr_length(str); i++) {
        printf("%c", str->data[i]);
    }
}

char *fstr_as_C(const fstr *from) {
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

/// Appends buf to strF
/// \param str The string being appended to
/// \param buf The string to be added
void fstr_append(fstr *str, const fstr *buf) {
    //Get / calculate the lengths that will be involved
    var startLength = fstr_length(str);
    var bufLength = fstr_length(buf);
    var newLength = startLength + bufLength;

    //Reallocate our string data to include room for our new buf
    str->data = realloc(str->data, newLength * sizeof(chr));

    //Copy the data from our buf onto our str data with an offset of our initial size in bytes
    memcpy(str->data + startLength * sizeof(chr), buf->data, bufLength);

    //Set the end to the pointer to the last character of our new string
    str->end = asptr(&str->data[newLength]);
}

/// Append a C style string to our str
/// \param str The string being appended to
/// \param buf The string to be added
void fstr_append_C(fstr *str, const char *buf) {
    var tmp = fstr_from_C(buf);
    fstr_append(str, tmp);
    free(tmp);
}

/// Creates a string filled with the chr fill with a length of length
/// \param length Total length of the string
/// \param fill The fill character
/// \return
fstr *fstr_from_length(uint64_t length, const chr fill) {

    //Error check
    if (length <= 0) {
        return fstr_from_C("");
    }

    //Create our string
    var str = fstr_from_C("");

    //Malloc new string data with the correct size
    str->data = malloc(length * sizeof(chr));

    //Set all the memory to our fill character
    memset(str->data, fill, length * sizeof(chr));

    //Set the end pointer to the last character of our stringF
    str->end = asptr(&str->data[length]);
}

void fstr_from_format(fstr *str, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int size = vsprintf_s(NULL, 0, format, args);


    va_end(args);
}

int main() {
//    var str = fstr_from_C(":)");
//
//    char buf[6] = {0};
//    for (int i = 0; i < 1000; i++) {
//        sprintf(buf, "%d\n", i);
//        fstr_append_C(str, buf);
//    }
//    fstr_slowprint(str);
//
//    fstr_free(str);


    var tmp = fstr_from_length(3, '0');
//    fstr_slowprint(tmp);

    printf("%s", fstr_as_C(tmp));

    return 0;
}
