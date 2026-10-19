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

__declspec(dllexport)
void pso_optimize(double *ref_img, double *cur_img, int width, int height,\
                     int population, int subset_side_len,\
                     double *img_ref_pt, double *img_cur_pt, double *result){
    if (ref_img == NULL || cur_img == NULL) return;
    struct sys_info *info = sys_create();
    if (!info) return;
    
    // initial image info
    info->dic_ctx.img_info.ref_data         = ref_img;
    info->dic_ctx.img_info.cur_data         = cur_img;
    info->dic_ctx.img_info.width            = width;
    info->dic_ctx.img_info.height           = height;
    info->dic_ctx.img_ref_pt_pos            = img_ref_pt;
    info->dic_ctx.img_cur_pt_pos            = img_cur_pt;
    info->pso_ctx.config.population         = population;
    info->pso_ctx.config.border_side_len 	= make_odd(height * PSO_CFG_BORDER_SCALE);
    info->dic_ctx.config.subset_side_len    = subset_side_len;

    // assign algo type
    info->pso_ctx.algo_ops                  = &standard_pso_algo_ops;

    if (info) {
        run_pso(info, cost_func_zncc);
        SYS_DBG("global_best_pos: (x,y) = (%.2f,%.2f)\n", info->pso_ctx.global_best.position[1], info->pso_ctx.global_best.position[0]);
        result[0] = info->pso_ctx.global_best.position[0]; // y
        result[1] = info->pso_ctx.global_best.position[1]; // x
        result[2] = info->pso_ctx.global_best.value; // coef
        sys_clean(info);
    }
}