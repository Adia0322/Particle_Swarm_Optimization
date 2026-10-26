#include "optim_problem.h"
#include "dic_problem.h"
#include "pso_math.h"
#include <stdint.h>
#include <stdlib.h>

int dic_problem_create(const struct dic_input_data *input, dic_cost_type type, struct optim_problem *out) {
    if (!input || !out) return DIC_FAILURE;
    double *dim_array_upper = malloc(sizeof(double) * DIC_DIMENSION);
    double *dim_array_lower = malloc(sizeof(double) * DIC_DIMENSION);

    if (!dim_array_upper || !dim_array_lower) goto clear_array;

    int border_len = make_odd(input->height * DIC_SEARCH_BORDER_SCALE );
    double half = (border_len - 1)/2.0;
    for (uint8_t idx=0; idx < DIC_DIMENSION; idx++) {
        dim_array_lower[idx] = -half;
        dim_array_upper[idx] = half;
    }

    *out = (struct optim_problem){
        .lower              = dim_array_lower,
        .upper              = dim_array_upper,
        .data               = NULL,
        .evaluate           = NULL,
        .destroy            = NULL,
        .direction          = OPTIM_MAXIMIZE,
        .dimension          = DIC_DIMENSION
    };

    switch(type) {
        case cost_func_zncc:
            out->data                      = zncc_create(input);
            out->evaluate                  = zncc_cost_function;
            out->destroy                   = zncc_cleanup;
            out->direction                 = OPTIM_MAXIMIZE;
            break;
            
        case cost_func_ssd:
            out->data                      = ssd_create(input);
            out->evaluate                  = ssd_cost_function;
            out->destroy                   = ssd_cleanup;
            out->direction                 = OPTIM_MINIMIZE;
            break;

        default:
            DBG_MSG("[ERROR] Not supported type!\n");
            goto clear_array;
    }

    if (!out->data) {
        goto data_malloc_fail;
    }
    if (zncc_init(out->data) != DIC_SUCCESS) {
        goto init_fail;
    }
    return DIC_SUCCESS;

data_malloc_fail:
    DBG_MSG("[ERROR] out->data malloc failed!\n");
    free(dim_array_upper);
    free(dim_array_lower);
    return DIC_FAILURE;

init_fail:
    DBG_MSG("[ERROR] PSO cost_func init failed!\n");
    free(dim_array_upper);
    free(dim_array_lower);
    zncc_cleanup(out->data);
    return DIC_FAILURE;

clear_array:
    DBG_MSG("[ERROR] clear_array!\n");
    free(dim_array_upper);
    free(dim_array_lower);
    return DIC_FAILURE;
}

void dic_problem_destroy(struct optim_problem *problem) {
    if (!problem) return;
    if (problem->data && problem->destroy) {
        problem->destroy(problem->data);
    }
    free((void *)problem->lower);
    free((void *)problem->upper);
    *problem = (optim_problem){0};
}