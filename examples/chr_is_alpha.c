//
// Created by tobin on 3/29/2024.
//

#include <stdio.h>
#include "../fstr.h"

int main(){
    printf("%d\n", chr_is_alpha('a'));
    printf("%d\n", chr_is_alpha('1'));
    printf("%d\n", chr_is_alpha(')'));
}