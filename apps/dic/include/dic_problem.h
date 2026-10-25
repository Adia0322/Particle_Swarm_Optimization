#ifndef DIC_COMM_H
#define DIC_COMM_H

#include <stdint.h>
#include <stdlib.h>

int dic_problem_create(
    const dic_input_data *input,
    dic_cost_type type,
    struct optim_problem *out);

void dic_problem_destroy(optim_problem *problem);

#endif