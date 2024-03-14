/*
 __/\\\\\\\\\\\\\\\________________________________________________________________________/\\\\\\\\\\\________________________________________________________________
 _\/\\\///////////_______________________________________________________________________/\\\/////////\\\______________________________________________________________
  _\/\\\___________________________________________________________/\\\__/\\\____________\//\\\______\///______/\\\_____________________/\\\_________________/\\\\\\\\__
   _\/\\\\\\\\\\\______/\\\\\\\\\_____/\\/\\\\\\_______/\\\\\\\\___\//\\\/\\\______________\////\\\__________/\\\\\\\\\\\__/\\/\\\\\\\__\///___/\\/\\\\\\____/\\\////\\\_
    _\/\\\///////______\////////\\\___\/\\\////\\\____/\\\//////_____\//\\\\\__________________\////\\\______\////\\\////__\/\\\/////\\\__/\\\_\/\\\////\\\__\//\\\\\\\\\_
     _\/\\\_______________/\\\\\\\\\\__\/\\\__\//\\\__/\\\_____________\//\\\______________________\////\\\______\/\\\______\/\\\___\///__\/\\\_\/\\\__\//\\\__\///////\\\_
      _\/\\\______________/\\\/////\\\__\/\\\___\/\\\_\//\\\_________/\\_/\\\________________/\\\______\//\\\_____\/\\\_/\\__\/\\\_________\/\\\_\/\\\___\/\\\__/\\_____\\\_
       _\/\\\_____________\//\\\\\\\\/\\_\/\\\___\/\\\__\///\\\\\\\\_\//\\\\/________________\///\\\\\\\\\\\/______\//\\\\\___\/\\\_________\/\\\_\/\\\___\/\\\_\//\\\\\\\\__
        _\///_______________\////////\//__\///____\///_____\////////___\////____________________\///////////_________\/////____\///__________\///__\///____\///___\////////___
 */

#include <stdint.h>

#ifndef FSTR_FSTR_H
#define FSTR_FSTR_H

#define PTR_SIZE uint64_t
#define chr char


/////////////////////////////
///      DEFINITIONS      ///
/////////////////////////////
#pragma region Definitions

///String errors, these will be updated on the fstr struct that you are using, in the error field
typedef enum : uint8_t {
    STR_ERR_None = 0,
    STR_ERR_IndexOutOfBounds = 1,
    STR_ERR_AllocFailed = 2,
    STR_ERR_ReallocFailed = 3,
    STR_ERR_NullStringArg = 4,
    STR_ERR_INCORRECT_CHAR_POINTER = 5
} STR_ERR;

///The main fstr struct, this is a new string type that uses a pointer to the end of the char array to control for length.
typedef struct {
    //The address of the last character in our string, inclusive.
    PTR_SIZE end;

    //Whether an error occurred on the string operation
    STR_ERR error;

    //The starting pointer of our string data
    chr *data;
} fstr;

#pragma endregion Definitions

/////////////////////////////
/// FUNCTION DECLARATIONS ///
/////////////////////////////

#pragma region String_Creation

/// Creates an fstr from a pre-existing C string
/// \param buf
/// \return
fstr *fstr_from_C(const char *buf);

/// Creates a fstr from a C format string
/// \param format Like printf
/// \param ... Args
/// \return
fstr *fstr_from_format_C(const char *format, ...);

/// Creates a string filled with the chr fill with a length of length
/// \param length Total length of the string
/// \param fill The fill character
/// \return
fstr *fstr_from_length(uint64_t length, chr fill);

#pragma endregion String_Creation

#pragma region String_Append

/// Append a C style string to our str
/// \param str The string being appended to
/// \param buf The string to be added
void fstr_append_C(fstr *str, const char *buf);

/// Appends buf to strF
/// \param str The string being appended to
/// \param buf The string to be added
void fstr_append(fstr *str, const fstr *buf);

/// Insert a string at a particular point
/// \param str The string to be modified
/// \param add The string to be added
/// \param index 0 inserts the string before any other data
void fstr_insert(fstr *str, const fstr *add, PTR_SIZE index);

/// Inserts a C string at a particular point, see fstr_insert
/// \param str The string to be modified
/// \param add The C string to be added
/// \param index 0 inserts the string before any other data
void fstr_insert_c(fstr *str, const char *add, PTR_SIZE index);

/// Appends a formatted C string to the fstr
/// \param str The string to be appended to
/// \param format The C string format of the data to be added
/// \param ... Takes varargs
void fstr_append_format_C(fstr *str, const char *format, ...);

/// Pads the string to fit a target length
/// \param str
/// \param targetLength uint64_t width of max pad
/// \param pad The character to pad
/// \param side -1 for pad left, 0 for pad both, 1 for pad right
void fstr_pad(fstr *str, PTR_SIZE targetLength, char pad, int8_t side);

#pragma endregion String_Append

#pragma region String_Modification

/// Replaces any instances of the from character with the to character
/// \param str
/// \param from
/// \param to
void fstr_replace_chr(fstr *str, chr from, chr to);

void fstr_replace_chr_at(fstr *str, PTR_SIZE index, chr c);

#pragma endregion String_Modification

#pragma region String_Utilities

/// Returns the fstr as a C string. This string MUST later be freed.
/// \param from The string to be used
/// \return The char * buffer
char *fstr_as_C_heap(const fstr *from);

/// Prints the string one character at a time
/// \param str The string to be printed
void fstr_print_slow(const fstr *str);

/// Prints the string at once by writing to the STDOUT
/// \param str The string to be printed
void fstr_print(const fstr *str);

/// Frees the fstr and its data
/// \param str The string to free
void fstr_free(fstr *str);

/// Returns the length of the string
/// \param str The corresponding string
/// \return The length of the string. Ex: ":)" returns 2
PTR_SIZE fstr_length(const fstr *str);

/// Whether or not the string is in a state of error
/// \param str
/// \return
uint8_t fstr_succeeded(fstr *str);

/// If the a strings contents equals the b strings contents
/// \param a The first string
/// \param b The second string
/// \return 1 if it equals, 0 if its not equal
uint8_t fstr_equals(fstr *a, fstr *b);

#pragma endregion String_Utilities

#endif //FSTR_FSTR_H