#include "common/include/data.h"


void no_opt halt()
{
    do {
        __asm__ __volatile__
        (
            "hlt;"
            :
            :
        );
    } while (1);
}

