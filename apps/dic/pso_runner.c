#include "optim_problem.h"
#include "dic_problem.h"
#include "pso_comm.h"
#include "pso_math.h"
#include <stdint.h>

__declspec(dllexport)
void pso_optimize(double *ref_img, double *cur_img, int width, int height,\
                     int population, int subset_side_len,\
                     double *img_ref_pt, double *img_cur_pt, double *result){
    if (!ref_img || !cur_img || !img_ref_pt || !img_cur_pt || !result) return;
    struct pso_context pso_ctx = {0};
    pso_ctx.config.population               = population;
    // pso_ctx.config.border_side_len 	        = make_odd(height * DIC_SEARCH_BORDER_SCALE );

    struct dic_input_data input = {
        .ref_img                            = ref_img,
        .cur_img                            = cur_img,
        .img_ref_pt                         = img_ref_pt,
        .img_cur_pt                         = img_cur_pt,
        .result                             = result,
        .width                              = width,
        .height                             = height,
        .population                         = population,
        .subset_side_len                    = subset_side_len
    };

    struct optim_problem problem = {0};
    if (dic_problem_create(&input, cost_func_zncc, &problem) != 0) {
        ERR_MSG("dic_problem_create failed!\n");
        return;
    }

    pso_ctx.problem                         = &problem;
    pso_ctx.algo_ops                        = &standard_pso_algo_ops;

    int ret = execute_pso_algorithm(&pso_ctx);
    if (ret == PSO_SUCCESS) {
        DBG_MSG("global_best_pos: (x,y) = (%.2f,%.2f)\n", pso_ctx.global_best.position[1], pso_ctx.global_best.position[0]);
        result[0] = pso_ctx.global_best.position[0]; // y
        result[1] = pso_ctx.global_best.position[1]; // x
        result[2] = pso_ctx.global_best.value;
    }
    dic_problem_destroy(&problem);
}
