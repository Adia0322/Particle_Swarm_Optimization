#include "system_comm.h"
#include "pso_comm.h"

int pso_create_factory(struct pso_context *pso_ctx, struct dic_input_data *input, pso_cost_type type) {
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
            SYS_DBG("[ERROR] Not supported type!\n");
            return PSO_FAILURE;
    }

    if (!pso_ctx->priv) {
        SYS_DBG("[ERROR] pso_ctx->priv malloc failed!\n");
        pso_destroy(pso_ctx); // ingore ret
        return PSO_FAILURE;
    }
    if (pso_ctx->cost_ops->init(pso_ctx->priv) != PSO_SUCCESS) { // zncc_init
        SYS_DBG("[ERROR] PSO cost_func init failed!\n");
        pso_destroy(pso_ctx);
        return PSO_FAILURE;
    }
    return PSO_SUCCESS;
}

int pso_destroy(struct pso_context *pso_ctx) {
    if (!pso_ctx) return PSO_FAILURE;

    if (pso_ctx->priv) {
        if (!pso_ctx->cost_ops || !pso_ctx->cost_ops->cleanup) return PSO_FAILURE;

        int ret = pso_ctx->cost_ops->cleanup(pso_ctx->priv);
        if (ret != PSO_SUCCESS) {
            SYS_DBG("[ERROR] cleanup(pso_ctx->priv) failed!\n");
            return PSO_FAILURE;
        }

        pso_ctx->priv = NULL;
    }

    pso_ctx->cost_ops = NULL;
    return PSO_SUCCESS;
}
