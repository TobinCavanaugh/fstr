//
// Created by tobin on 3/18/2024.
//

#ifndef FSTR_FSTR_H

#include "fstr.h"

#endif //FSTR_FSTR_H

#ifndef FSTR_FSTR_PARSE_H
#define FSTR_FSTR_PARSE_H

/// Tries to parse the string to a signed 64 bit integer.
/// Ignores leading whitespaces. In the case of failure, the value at 'outValue' is set to the parsed integer up until that point
/// \param str The string to read from
/// \param outValue A pointer to the value to be set
/// \return 1 for success, 0 for failure.
uint8_t fstr_try_to_i64(const fstr *str, int64_t *outValue);

/// Returns the parsed value, whether or not it fails.
/// \see Safer Method: fstr_try_to_i64
/// \param str The string to be read from
/// \return The parsed value
int64_t fstr_to_i64(const fstr *str);

uint8_t fstr_try_to_u64(const fstr *str, uint64_t *outValue);

uint64_t fstr_to_u64(const fstr *str);

#endif //FSTR_FSTR_PARSE_H