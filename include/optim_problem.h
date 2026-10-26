#ifndef OPTIM_PROBLEM_H
#define OPTIM_PROBLEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

typedef enum {
    OPTIM_MINIMIZE,
    OPTIM_MAXIMIZE
} optim_direction;

typedef uint8_t optim_direction_t;

typedef struct optim_problem {
    const double *lower;
    const double *upper;
    double (*evaluate)(void *data, const double *position);
    int (*destroy)(void *data);
    void *data;
    optim_direction_t direction;
    uint8_t  dimension;
} optim_problem;

#endif
