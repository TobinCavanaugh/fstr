//
// Created by tobin on 3/29/2024.
//

#include "../fstr.h"

int main(){
    fstr * str = fstr_from_C("abc123");
    fstr_println(str);
    fstr_free(str);
}