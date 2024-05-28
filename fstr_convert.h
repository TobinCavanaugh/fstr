//
// Created by tobin on 3/18/2024.
//

#ifndef FSTR_FSTR_H

#include "fstr.h"

#endif //FSTR_FSTR_H

#ifndef FSTR_FSTR_PARSE_H
#define FSTR_FSTR_PARSE_H

/// Calculate an i_val from a string
/// @param str The string
/// @return An fstr_result with the result in i_val.
/// Sides will be trimmed of spaces, non numerical or negative
/// sign characters cause success to be false, though the i_val
/// will be the correct value up until that character.
fstr_result fstr_to_i64(const fstr* str);

/// Calculate an f_val from a string
/// @param str The string
/// @return An fstr_result with the result in f_val.
/// Sides will be trimmed of spaces, non numerical, negative,
/// or decimal characters will cause success to be false,
/// though the value will be correct up until that character.
fstr_result fstr_to_double(fstr* str);

/// Calculates a u_val from a binary string, like so "10100001" or "0b0101"
/// @param str The string of the binary
/// @return An fstr_result with the value in u_val.
/// Any non-binary !('0'|'1') will be skipped and the value will
/// continue to be calculated
fstr_result fstr_u64_from_bin(fstr* str);

/// Calculates a u_val from a binary string, like so "10100001" or "0b0101"
/// @param str The string of the binary
/// @return An fstr_result with the value in u_val.
/// Any non-binary !('0'|'1') will be skipped and the value will
/// continue to be calculated. This extension method allows customs chrs for
/// 1 and 0, being true and false respectively. Use the fstr_u64_from_bin for
/// these to be set to the defaults, '1' and '0'
fstr_result fstr_u64_from_bin_ex(fstr* instr, chr True, chr False);

#endif //FSTR_FSTR_PARSE_H
