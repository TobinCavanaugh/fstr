#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#define var __auto_type
#define PTR_SIZE uint64_t
#define chr char
#define u8 uint8_t

#define asptr(a) ((PTR_SIZE) a)

typedef enum {
    None = 0,
    IndexOutOfBounds = 1,
    AllocFailed = 2,
    NullString,
} STRING_ERROR;

//TODO MEMORY ARENA MACROS, OVERRIDE MALLOC, IMPLY ENDING THING

typedef struct {
    //The address of the last character in our string, inclusive.
    PTR_SIZE end;

    //Whether an error occurred on the string operation
    STRING_ERROR error;

    //The starting pointer of our string data
    chr *data;
} fstr;


/// Sets the end pointer of the fstr
/// \param str
/// \param newLength
void _internal_fstr_set_end(fstr *str, PTR_SIZE newLength) {
    str->end = asptr(&str->data[newLength]);
}

/// Custom String Length function
/// \param buf The string to check
/// \return The length of the string, NOT including the null terminator
unsigned long long _internal_C_string_length(const char *buf) {
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
    var bufSize = _internal_C_string_length(buf) * sizeof(chr);

    //Malloc our struct
    fstr *str = malloc(sizeof(fstr));

    //Malloc our data
    str->data = malloc(bufSize);

    //Reset error
    str->error = 0;

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

/// Returns the fstr as a C string. This string MUST later be freed.
/// \param from
/// \return
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
    memcpy(str->data + startLength * sizeof(chr), buf->data, bufLength * sizeof(chr));

    //Set the end to the pointer to the last character of our new string
    _internal_fstr_set_end(str, newLength);
}

/// Append a C style string to our str
/// \param str The string being appended to
/// \param buf The string to be added
void fstr_append_C(fstr *str, const char *buf) {
    var startLen = fstr_length(str);
    var bufLen = _internal_C_string_length(buf);
    var newLen = startLen + bufLen;

    str->data = realloc(str->data, newLen * sizeof(chr));

    memcpy(str->data + startLen * sizeof(chr), buf, bufLen * sizeof(chr));
    _internal_fstr_set_end(str, newLen);

    //    var tmp = fstr_from_C(buf);
//    fstr_append(str, tmp);
//    free(tmp);
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

    //Set our error to 0
    str->error = 0;

    //Set all the memory to our fill character
    memset(str->data, fill, length * sizeof(chr));

    //Set the end pointer to the last character of our stringF
//    str->end = asptr(&str->data[length]);
    _internal_fstr_set_end(str, length);
}

/// Creates a fstr from a C format string
/// \param format Like printf
/// \param ... Args
/// \return
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


/// Appends a formatted C string to the fstr
/// \param str The string to be appended to
/// \param format The C string format of the data to be added
/// \param ... Takes varargs
void fstr_append_format_C(fstr *str, const char *format, ...) {

    va_list args;
    va_start(args, format);

    //Get the sizes
    var startSize = fstr_length(str) * sizeof(chr);
    var addSize = _vscprintf(format, args) / sizeof(chr);
    var finalSize = startSize + addSize;

    //Reallocate the data of the string to fit our newsize
    str->data = realloc(str->data, finalSize * sizeof(chr));

    //Do our vsprintf to the data, offset by our start size as to write our data to our unneeded stuff
    vsprintf(str->data + startSize, format, args);

    //Set the end of our string
    _internal_fstr_set_end(str, finalSize / sizeof(chr));

    va_end(args);
}

/// Replaces any instances of the from character with the to character
/// \param str
/// \param from
/// \param to
void fstr_replace_char(fstr *str, chr from, chr to) {

    var len = fstr_length(str);

    for (int i = 0; i < len; i++) {
        if (str->data[i] == from) {
            str->data[i] = to;
        }
    }
}

/// Returns 1 if the strings match and 0 if they do not.
/// \param a
/// \param b
/// \return
uint8_t fstr_equals(fstr *a, fstr *b) {
    var alen = fstr_length(a);
    var blen = fstr_length(b);

    if (alen != blen) {
        return 0;
    }

    for (int i = 0; i < alen; i++) {
        if (a->data[i] != b->data[i]) {
            return 0;
        }
    }

    return 1;
}

/// Pads the string to fit a target length
/// \param str
/// \param targetLength
/// \param side -1 for pad left, 0 for pad both, 1 for pad right
void fstr_pad(fstr *str, PTR_SIZE targetLength, int8_t side) {

}

///
/// \param str
/// \param add
/// \param index 0 inserts the string before any other data
void fstr_insert(fstr *str, const fstr *add, PTR_SIZE index) {
    var startLen = fstr_length(str);
    var addLen = fstr_length(add);
    var finalLen = startLen + addLen;

    if (addLen == 0) {
        return;
    }

    if (index >= finalLen) {
        str->error = 1;
        return;
    }

    str->data = realloc(str->data, finalLen * sizeof(chr));

    //[A][B][C][D][E][F] <-(2) [Z][X][Y]

    //Shift the first part of the buffer over
    //[A][B][-][-][-][C][D][E][F]
    memcpy(str->data + (index + addLen) * sizeof(chr), str->data + index, (startLen - index) * sizeof(chr));

    //[A][B][-][-][-][C][D][E][F]
    memcpy(str->data + index, add->data, addLen * sizeof(chr));

//    memcpy(str->data + index * sizeof(chr), add->data, addLen * sizeof(chr));

    _internal_fstr_set_end(str, finalLen);
}

u8 fstr_succeeded(fstr *str) {
    u8 err = str->error;
    str->error = 0;
    return err;
}

void start_stopwatch(struct timeval *start_time) {
    gettimeofday(start_time, NULL);
}

double stop_stopwatch(struct timeval start_time) {
    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0; // seconds to milliseconds
    elapsed_time += (end_time.tv_usec - start_time.tv_usec) / 1000.0; // microseconds to milliseconds
    return elapsed_time;
}

int main() {

    var fstr_string = fstr_from_format_C("%d%d", 10, 20);
    var add = fstr_from_C("AB");
    fstr_insert(fstr_string, add, 2);

    fstr_slowprint(fstr_string);

    return 0;

    int F_ITER = 10;
    int BENCHMARK_ITER = 1;
    char *ADDITION = "AA";

    for (int i = 0; i < BENCHMARK_ITER; i++) {

        struct timeval start_time = {};
        start_stopwatch(&start_time);

        var myStr = fstr_from_C("");

        for (uint64_t r = 0; r < F_ITER; r++) {
            fstr_append_C(myStr, ADDITION);
        }

        var fstrTime = stop_stopwatch(start_time);

//        fstr_slowprint(myStr);

        start_stopwatch(&start_time);

        char *cStr = malloc(1 * sizeof(char));
        cStr[0] = '0';

        for (int r = 0; r < F_ITER; r++) {
            cStr = realloc(cStr, strlen(cStr) + strlen(ADDITION) + 2);
            cStr[r] = '\0';
            strcat(cStr, ADDITION);
        }

        cStr[F_ITER - 1] = '\0';


        var cTime = stop_stopwatch(start_time);
        printf("%.3fms - %.3fms == %.3fms\n", fstrTime, cTime, fstrTime - cTime);

        printf("\nFSTR:");
        fstr_slowprint(myStr);

        printf("\n\n\n");

        printf("\nCSTR:");
        printf("%s", cStr);

        fstr_free(myStr);
        free(cStr);

        printf("\n----------------------------------------------------------------------\n");
    }


    return 0;
}
