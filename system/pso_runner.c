#include "system_comm.h"
#include "PSO_comm.h"

void run_pso(struct sys_info *info, pso_cost_type type) {
	// create pso_context and put DIC info in it
	pso_create_factory(info, type);

	if (!info->pso_ctx.priv) return;

    if (execute_pso_algorithm(&info->pso_ctx) != 0) {
        SYS_DBG("execute_pso_algorithm failed!!\n");
        return;
    }
}