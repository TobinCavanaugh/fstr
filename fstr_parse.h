//
// Created by tobin on 3/18/2024.
//

#ifndef FSTR_FSTR_H

#include "fstr.h"

#endif //FSTR_FSTR_H

#ifndef FSTR_FSTR_PARSE_H
#define FSTR_FSTR_PARSE_H

#define bool uint8_t

usize fstr_get_parse_error();

uint8_t fstr_try_to_i64(const fstr *str, int64_t *out);

#endif //FSTR_FSTR_PARSE_H
