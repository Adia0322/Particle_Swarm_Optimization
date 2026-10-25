#ifndef OPTIM_PROBLEM_H
#define OPTIM_PROBLEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "dic_comm.h"
#include "pso_comm.h"

typedef enum {
    OPTIM_MINIMIZE,
    OPTIM_MAXIMIZE
} optim_direction;

typedef uint8_t optim_direction_t;

typedef struct optim_problem {
    const double *lower;
    const double *upper;
    void *data;
    double (*evaluate)(void *data, const double *position);
    void (*destroy)(void *data);

    optim_direction_t direction;
    uint8_t  dimension;
} optim_problem;

int cost_setup(struct pso_context *pso_ctx, struct dic_input_data *input, dic_cost_type type);
int cost_cleanup(struct pso_context *pso_ctx);
#endif
