#include <stdlib.h>
#include <time.h>
#include "pso_math.h"
#include "dic_comm.h"

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

double get_mean(double sum, int side_len) {
    return sum/(double)((side_len)*(side_len)); // return type: double
}

int make_odd(int value) {
    return (value % 2 == 0) ? value + 1 : value;
}
