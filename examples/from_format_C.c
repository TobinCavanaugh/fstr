//
// Created by tobin on 3/29/2024.
//

#include "../fstr.h"

int main(){
    fstr * str = fstr_from_format_C("%02d-%02d-%d", 4, 12, 2005);
    fstr_println(str);
    fstr_free(str);
}
