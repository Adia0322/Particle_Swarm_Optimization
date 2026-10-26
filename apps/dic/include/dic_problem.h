#ifndef DIC_PROBLEM_H
#define DIC_PROBLEM_H

#include <stdint.h>
#include <stdlib.h>
#include <optim_problem.h>
#include "dic_comm.h"

int dic_problem_create(
    const struct dic_input_data *input,
    dic_cost_type type,
    struct optim_problem *out);

void dic_problem_destroy(optim_problem *problem);

#endif