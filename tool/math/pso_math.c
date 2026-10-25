#include <stdlib.h>
#include <time.h>
#include "pso_math.h"

double random_digit(void)
{
    return rand() / (double)RAND_MAX;
}

void init_random_seed() {
    static int seeded = 0;
    if (!seeded) {
        time_t t = time(NULL);
        srand((unsigned) t);
        seeded = 1;
    }
}

int make_odd(int value) {
    return (value % 2 == 0) ? value + 1 : value;
}
