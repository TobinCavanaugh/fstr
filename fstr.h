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
#include <wchar.h>

#ifndef FSTR_FSTR_H
#define FSTR_FSTR_H

/////////////////////////////
///      DEFINITIONS      ///
/////////////////////////////
#pragma region Definitions

///usize is our common size to be used for string lengths, indexes, addresses, and the like
#define usize uintptr_t

///We use chr, the char type can be changed to wchar_t and most functionality will remain.
#define chr char
//#define chr wchar_t

///String errors, these will be updated on the fstr struct that you are using, in the error field
typedef enum : uint8_t {
    STR_ERR_None = 0,
    STR_ERR_IndexOutOfBounds = 1,
    STR_ERR_AllocFailed = 2,
    STR_ERR_ReallocFailed = 3,
    STR_ERR_NullStringArg = 4,
    STR_ERR_INCORRECT_CHAR_POINTER = 5
} STR_ERR;

//This struct is used to handle the results of functions that might return different things.
//In particular, it has the possibility of returning a boolean telling you if the function succeeded or not.
typedef struct {
    uint8_t success;

    //This union defines either an integer value or a float value.
    //Most functions will return an u_val (index value) unless specified.
    union {
        usize u_val;
        int64_t i_val;
        double f_val;
    };
} fstr_result;

///The main fstr struct, this is a new string type that uses a pointer to the end of the char array to control for length.
typedef struct {
    //The address of the last character in our string, inclusive.
    usize end;

    //Whether an error occurred on the string operation
    STR_ERR error;

    //The starting pointer of our string data
    chr *data;
} fstr;

#pragma endregion Definitions

/////////////////////////////
/// FUNCTION DECLARATIONS ///
/////////////////////////////

#pragma region Character

#define USING_WCHAR (sizeof(chr) == sizeof(wchar_t))
#define USING_CHAR (sizeof(chr) == sizeof(char))

//There will be NO ASCII extended case support, as it is not standardized
#define chr_is_lower(a) (USING_CHAR && (a >= 97 && a <= 122)) || (USING_WCHAR && (iswlower(a)))
#define chr_is_upper(a) (USING_CHAR && (a >= 65 && a <= 90)) || (USING_WCHAR && (iswupper(a)))
#define chr_is_alpha(a) (chr_is_lower(a) || chr_is_upper(a))

chr chr_to_lower(chr a);

chr chr_to_upper(chr a);



#pragma endregion Character

#pragma region String_Creation

/// Creates an fstr from a pre-existing C string
/// \param buf
/// \return
fstr *fstr_from_C(const chr *buf);

/// Creates a fstr from a C format string
/// \param format Like printf
/// \param ... Args
/// \return
fstr *fstr_from_format_C(const char *format, ...);

/// Creates a string filled with the chr fill with a length of length
/// \param length Total length of the string
/// \param fill The fill character
/// \return
fstr *fstr_from_length(usize length, const chr fill);

#pragma endregion String_Creation

#pragma region String_Append

/// Append a C style string to our str
/// \param str The string being appended to
/// \param buf The string to be added
void fstr_append_C(fstr *str, const chr *buf);

/// Appends the fstr buf to the fstr
/// \param str The string being appended to
/// \param buf The string to be added
void fstr_append(fstr *str, const fstr *buf);

/// Appends a single character to the string
/// \param str The string being appended to
/// \param c The character to be appended
void fstr_append_chr(fstr *str, const chr c);

/// Insert a string at a particular point
/// \param str The string to be modified
/// \param add The string to be added
/// \param index 0 inserts the string before any other data
void fstr_insert(fstr *str, usize index, const fstr *add);

/// Inserts a C string at a particular point, see fstr_insert
/// \param str The string to be modified
/// \param add The C string to be added
/// \param index 0 inserts the string before any other data
void fstr_insert_C(fstr *str, usize index, const chr *add);

/// Appends a formatted C string to the fstr
/// \param str The string to be appended to
/// \param format The C string format of the data to be added
/// \param ... Takes varargs
void fstr_append_format_C(fstr *str, const char *format, ...);

/// Pads the string to fit a target length
/// \param str
/// \param targetLength width of max pad
/// \param pad The character to pad
/// \param side -1 for pad left, 0 for pad both, 1 for pad right
void fstr_pad(fstr *str, usize targetLength, chr pad, int8_t side);

#pragma endregion String_Append

#pragma region Printing

/// Prints the characters as hex codes separated by spaces
/// \param str The string to be printed
void fstr_print_hex(const fstr *str);

/// Prints the string then a newline
/// \param str The string to be printed
void fstr_println(const fstr *str);

/// Prints the chrs with a format applied to each character. This MUST include %c and cannot include any other % formatting.
/// A good use for this is fstr_print_chrs_f(str, "%c,"); To comma separate the characters.
/// \param str
/// \param format
void fstr_print_chrs_f(const fstr *str, const chr *format);

/// Prints the string one character at a time
/// \param str The string to be printed
void fstr_print_chrs(const fstr *str);

/// Prints the string at once by writing to the STDOUT
/// \param str The string to be printed
void fstr_print(const fstr *str);

#pragma endregion Printing

#pragma region String_Modification

/// Returns the index of the first instances of the substring in the string. Index is stored in fstr_result.i_val
/// \param str The string to look through
/// \param sub The substring to check for
/// \return The fstr_result, index is stored in i_val.
fstr_result fstr_index_of_C(const fstr *str, chr *sub);

/// Returns the index of the first instances of the substring in the string. Index is stored in fstr_result.i_val
/// \param str The string to look through
/// \param sub The substring to check for
/// \return The fstr_result, index is stored in i_val.
fstr_result fstr_index_of(const fstr *str, const fstr *sub);

/// Clear all the string characters
/// \param str The string to clear
void fstr_clear(fstr *str);

/// Removes the character at the particular index
/// \param str The string to be modified
/// \param index The index of the char to be removed, 0 based. Will not crash on OOB
void fstr_remove_at(fstr *str, const usize index, const usize length);

/// Reverses the string
/// \param str The string to be reversed
void fstr_reverse(fstr *str);

/// Removes any instances of the fstr buf in the str
/// \param str The source string
/// \param buf The buf to remove from the str
void fstr_remove(const fstr *str, const fstr *buf);

/// Removes any instances of the chr array buf in the str
/// \param str The source string
/// \param buf The buf to remove from the str
void fstr_remove_C(const fstr *str, const chr *buf);

/// Replaces any instances of the from character with the to character
/// \param str The source string
/// \param from The chr as its found in the source
/// \param to The new chr to replace the from chr
void fstr_replace_chr(fstr *str, const chr from, const chr to);

/// Removes any instances of a character, rippling the string. fstr_remove_chr(str, "AABBCC", 'A') -> "BBCC"
/// \param str The string to be removed c
/// \param c The chr to be removed
void fstr_remove_chr(fstr *str, const chr c);

/// Replaces the character at a particular index, you can also do direct indexing. Does do OOB checking.
/// \param str The string to replace
/// \param index The index of the character, 0 being the start of the string
/// \param c The character to be assigned
void fstr_set_chr(fstr *str, usize index, chr c);

/// Removes all instances of the char parameters
/// \param str The string to be modified
/// \param num_chars The count of char params to be passed
/// \param ... Chars to be removed
void fstr_remove_chr_varargs(fstr *str, uint8_t num_chars, ...);

/// Makes the string lowercase
/// \param a The string to modify
void fstr_to_lower(fstr *a);

/// Makes the string uppercase
/// \param a The string to modify
void fstr_to_upper(fstr *a);

/// Makes any uppercase into lowercase, and any lowercase into uppercase
/// \param a The string to modify
void fstr_invertcase(fstr *a);

#pragma endregion String_Modification

#pragma region String_Utilities

/// Returns an fstr substring of the string, starting at start and with a length
/// \param str The string to be used
/// \param start The start index of the substring
/// \param length The length of the substring
/// \return The substring
fstr *fstr_substr(fstr *str, usize start, usize length);

/// Counts the instances of a C substring in str
/// \param str The string to search
/// \param sub The substring to look for
/// \return The count of substrings
usize fstr_count_C(const fstr *str, const chr *sub);

/// Counts the instances of an fstr substring in the string
/// \param str The string to search
/// \param sub The substring to look for
/// \return The count of substringsF
usize fstr_count(const fstr *str, const fstr *sub);

/// returns an fstr result if the character was found. The value is in the u_val
/// \param str The string to search
/// \param c The character to compare
/// \return The fstr_result with the value being the u_val
fstr_result fstr_index_of_chr(fstr *str, char c);

/// Gets the count of the chr value in the fstr
/// \param str The string to be checked
/// \param value The chr to check the string for
/// \return The count of characters
usize fstr_count_chr(const fstr *str, const chr value);

/// Copies the fstr and returns the new copy
/// \param str The string to be copied
/// \return A copy of the fstr
fstr *fstr_copy(const fstr *str);

/// Returns the fstr as a C string. This string MUST later be freed.
/// \param from The string to be used
/// \return The char * buffer
char *fstr_as_C_heap(const fstr *from);


/// Frees the fstr and its data
/// \param str The string to free
void fstr_free(fstr *str);

/// Returns the length of the string
/// \param str The corresponding string
/// \return The length of the string. Ex: ":)" returns 2
usize fstr_length(const fstr *str);

/// Whether or not the string is in a state of error
/// \param str
/// \return
uint8_t fstr_succeeded(fstr *str);

/// If the a strings contents equals the b strings contents
/// \param a The first string
/// \param b The second string
/// \return 1 if it equals, 0 if its not equal
uint8_t fstr_equals(fstr *a, fstr *b);

/// Trims a particular side of the fstr from all spaces, tabs, newlines, carriage returns, and the like.
/// \param str The string to be modified
/// \param side The side to trim, 0 for both sides, -1 for left, +1 for right
void fstr_trim(fstr *str, int8_t side);

/// Replaces any instances of oldBuf with newBuf
/// \param str The string to be modified
/// \param oldBuf The old buffer
/// \param newBuf The new buffer
void fstr_replace_C(const fstr *str, const chr *oldBuf, const chr *newBuf);

/// Replaces any instances of oldBuf with newBuf
/// \param str The string to be modified
/// \param oldBuf The old buffer
/// \param newBuf The new buffer
void fstr_replace(const fstr *str, const fstr *oldBuf, const fstr *newBuf);

#pragma endregion String_Utilities

#endif //FSTR_FSTR_H