#include "system_comm.h"
#include "pso_comm.h"
#include "pso_math.h"

__declspec(dllexport)
void pso_optimize(double *ref_img, double *cur_img, int width, int height,\
                     int population, int subset_side_len,\
                     double *img_ref_pt, double *img_cur_pt, double *result){
    if (ref_img == NULL || cur_img == NULL) return;
    struct pso_context pso_ctx = {0};
    pso_ctx.config.population               = population;
    pso_ctx.config.border_side_len 	        = make_odd(height * PSO_CFG_BORDER_SCALE);

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
    
    pso_ctx.algo_ops                        = &standard_pso_algo_ops;

    if (pso_create_factory(&pso_ctx, &input, cost_func_zncc) != 0) {
        SYS_DBG("[ERROR] pso_create_factory failed!\n");
        return;
    }
    if (!pso_ctx.priv) return;

    if (execute_pso_algorithm(&pso_ctx) != 0) {
        SYS_DBG("[ERROR] execute_pso_algorithm failed!\n");
        pso_destroy(&pso_ctx);
        return;
    }

    SYS_DBG("global_best_pos: (x,y) = (%.2f,%.2f)\n", pso_ctx.global_best.position[1], pso_ctx.global_best.position[0]);
    result[0] = pso_ctx.global_best.position[0]; // y
    result[1] = pso_ctx.global_best.position[1]; // x
    result[2] = pso_ctx.global_best.value; // coef

    if (pso_destroy(&pso_ctx) != 0) {
        SYS_DBG("[ERROR] pso_destroy failed!\n");
        return;
    }
}
