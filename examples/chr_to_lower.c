//
// Created by tobin on 3/29/2024.
//

#include <stdio.h>
#include "../fstr.h"

int main() {
    chr c = 'A';
    printf("%c->%c", c, chr_to_lower(c));
}