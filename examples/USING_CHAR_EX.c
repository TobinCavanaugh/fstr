//
// Created by tobin on 3/29/2024.
//

#include <stdio.h>
#include "../fstr.h"

void main() {
    if (USING_WCHAR) {
        printf("Using wchar_t");
    } else {
        printf("Using char");
    }
}
