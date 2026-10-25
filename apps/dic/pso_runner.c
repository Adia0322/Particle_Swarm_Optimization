#include "optim_problem.h"
#include "pso_comm.h"
#include "pso_math.h"

int cost_setup(struct pso_context *pso_ctx, struct dic_input_data *input, pso_cost_type type) {
    // SELECT PSO COST FUNC TYPE
    if (!pso_ctx || !input || pso_ctx->priv) return PSO_FAILURE;
    switch(type) {
        case cost_func_zncc:
            pso_ctx->priv                  = zncc_create(input);
            pso_ctx->cost_ops              = &zncc_ops;
            break;
            
        case cost_func_ssd:
            pso_ctx->priv                  = ssd_create(input);
            pso_ctx->cost_ops              = &ssd_ops;
            break;

        default:
            DBG_MSG("[ERROR] Not supported type!\n");
            return PSO_FAILURE;
    }

    if (!pso_ctx->priv) {
        DBG_MSG("[ERROR] pso_ctx->priv malloc failed!\n");
        cost_cleanup(pso_ctx); // ingore ret
        return PSO_FAILURE;
    }
    if (pso_ctx->cost_ops->init(pso_ctx->priv) != PSO_SUCCESS) { // zncc_init
        DBG_MSG("[ERROR] PSO cost_func init failed!\n");
        cost_cleanup(pso_ctx);
        return PSO_FAILURE;
    }
    return PSO_SUCCESS;
}

int cost_cleanup(struct pso_context *pso_ctx) {
    if (!pso_ctx) return PSO_FAILURE;

    if (pso_ctx->priv) {
        if (!pso_ctx->cost_ops || !pso_ctx->cost_ops->cleanup) return PSO_FAILURE;

        int ret = pso_ctx->cost_ops->cleanup(pso_ctx->priv);
        if (ret != PSO_SUCCESS) {
            DBG_MSG("[ERROR] cleanup(pso_ctx->priv) failed!\n");
            return PSO_FAILURE;
        }

        pso_ctx->priv = NULL;
    }

    pso_ctx->cost_ops = NULL;
    return PSO_SUCCESS;
}

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

    if (cost_setup(&pso_ctx, &input, cost_func_zncc) != 0) {
        DBG_MSG("[ERROR] cost_setup failed!\n");
        return;
    }
    if (!pso_ctx.priv) return;

    if (execute_pso_algorithm(&pso_ctx) != 0) {
        DBG_MSG("[ERROR] execute_pso_algorithm failed!\n");
        cost_cleanup(&pso_ctx);
        return;
    }

    DBG_MSG("global_best_pos: (x,y) = (%.2f,%.2f)\n", pso_ctx.global_best.position[1], pso_ctx.global_best.position[0]);
    result[0] = pso_ctx.global_best.position[0]; // y
    result[1] = pso_ctx.global_best.position[1]; // x
    result[2] = pso_ctx.global_best.value; // coef

    if (cost_cleanup(&pso_ctx) != 0) {
        DBG_MSG("[ERROR] cost_cleanup failed!\n");
        return;
    }
}
