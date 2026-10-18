#include "system_comm.h"
#include "PSO_comm.h"

void pso_create_factory(pso_cost_type type, struct sys_info *info) {
    // SELECT PSO COST FUNC TYPE
    if (type == PSO_TYPE_ZNCC) {
        struct dic_zncc_context *zncc_ctx   = malloc(sizeof(struct dic_zncc_context));
        info->pso_ctx.priv                  = zncc_ctx;
        info->pso_ctx.cost_ops              = &zncc_ops;
    } 
    else if (type == PSO_TYPE_SSD) {
        struct dis_ssd_context *ssd_ctx     = malloc(sizeof(struct dis_ssd_context));
        info->pso_ctx.priv                  = ssd_ctx;
        info->pso_ctx.cost_ops              = &ssd_ops;
    }

    if (info->pso_ctx.cost_ops->init(info) != 0) {
        SYS_DBG("PSO cost_func init failed!\n");
        free(info->pso_ctx.priv);
        return;
    }
}

int pso_destroy(struct pso_context *pso_ctx) {
    if (!pso_ctx) return PSO_FAILURE;
    if (pso_ctx->cost_ops && pso_ctx->cost_ops->cleanup) {
        pso_ctx->cost_ops->cleanup(pso_ctx->priv);
    }
    free(pso_ctx->priv);
    return PSO_SUCCESS;
}