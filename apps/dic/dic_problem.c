#include "optim_problem.h"
#include "dic_problem.h"
#include "pso_math.h"
#include <stdint.h>
#include <stdlib.h>

int dic_problem_create(const struct dic_input_data *input, dic_cost_type type, struct optim_problem *out) {
    if (!input || !out) return DIC_FAILURE;
    double *dim_array_upper = malloc(sizeof(double) * DIC_DIMENSION);
    double *dim_array_lower = malloc(sizeof(double) * DIC_DIMENSION);

    if (!dim_array_upper || !dim_array_lower) goto fail;

    int border_len = make_odd(input->height * DIC_SEARCH_BORDER_SCALE );
    double half = (border_len - 1)/2.0;
    for (uint8_t idx=0; idx < DIC_DIMENSION; idx++) {
        dim_array_lower[idx] = -half;
        dim_array_upper[idx] = half;
    }

    *out = (struct optim_problem){
        .lower              = dim_array_lower,
        .upper              = dim_array_upper,
        .evaluate           = NULL,
        .destroy            = NULL,
        .data               = NULL,
        .direction          = OPTIM_MAXIMIZE,
        .dimension          = DIC_DIMENSION
    };

    switch(type) {
        case cost_func_zncc:
            out->evaluate                  = zncc_cost_function;
            out->destroy                   = zncc_cleanup;
            out->data                      = zncc_ctx_create(input);
            out->direction                 = OPTIM_MAXIMIZE;
            break;
            
        case cost_func_ssd:
            out->evaluate                  = ssd_cost_function;
            out->destroy                   = ssd_cleanup;
            out->data                      = ssd_ctx_create(input);
            out->direction                 = OPTIM_MINIMIZE;
            break;

        default:
            DBG_MSG("[ERROR] Not supported type!\n");
            goto fail;
    }

    if (!out->data) {
        DBG_MSG("[ERROR] out->data malloc failed!\n");
        goto fail;
    }
    return DIC_SUCCESS;

fail:
    free(dim_array_upper);
    free(dim_array_lower);
    out->destroy(out->data);
    *out = (struct optim_problem){0};
    return DIC_FAILURE;
}

void dic_problem_destroy(struct optim_problem *problem) {
    if (!problem) return;
    if (problem->data && problem->destroy) {
        problem->destroy(problem->data); // destroy need exist
    }
    free((void *)problem->lower);
    free((void *)problem->upper);
    *problem = (optim_problem){0};
}
