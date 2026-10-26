#include "optim_problem.h"
#include "pso_comm.h"
#include "pso_math.h"
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

static inline double rand0_1(void) {
    return (double)rand() * (1.0 / (double)RAND_MAX);
}

static inline void clamp_to_bounds(double *value, double max, double min) {
	if (!value) return;
	if (*value > max) *value = max;
	else if (*value < min) *value = min;
}

int execute_pso_algorithm(struct pso_context *ctx) {
	if (!ctx || !ctx->algo_ops || !ctx->algo_ops->run || !ctx->algo_ops->init || !ctx->algo_ops->cleanup) {
		return PSO_FAILURE;
	}
	int ret = PSO_FAILURE;

	if (ctx->algo_ops->init(ctx) != PSO_SUCCESS) { // st_pso_algo_init
		DBG_MSG("algo init failed!\n");
		goto cleanup;
	}

	if (ctx->algo_ops->run(ctx) != PSO_SUCCESS) { // st_pso_algo_run
		DBG_MSG("algo run failed!\n");
		goto cleanup;
	}
	ret = PSO_SUCCESS;

cleanup:
	if (ctx->algo_ops->cleanup(ctx) != PSO_SUCCESS) {
		DBG_MSG("algo cleanup failed!\n");
		ret = PSO_FAILURE;
	}

	return ret;
}

void st_pso_config_init(struct pso_context *ctx) {
    // ctx->config.population 				= 		PSO_CFG_POPULATION; // depends on input
    ctx->config.dimension 					= 		PSO_CFG_DIMENSION;
    ctx->config.max_iter 					= 		PSO_CFG_MAX_ITER; 
    ctx->config.v_max 						= 		PSO_CFG_V_MAX;
    ctx->config.v_min 						= 		PSO_CFG_V_MIN;
	ctx->config.v_ini						=		PSO_CFG_V_INI;
    ctx->config.w_max 						= 		PSO_CFG_W_MAX;
    ctx->config.w_min 						= 		PSO_CFG_W_MIN;
    ctx->config.c1 							= 		PSO_CFG_C1;
    ctx->config.c2 							=		PSO_CFG_C2;
    ctx->config.dec_factor 					= 		PSO_CFG_DEC_FACTOR;
    ctx->config.inc_factor 					=		PSO_CFG_INC_FACTOR;
	ctx->global_best.value					=		ctx->problem->direction == OPTIM_MAXIMIZE ? -INFINITY : INFINITY;
}

void st_pso_layout_init(struct pso_context *ctx) {
	// init st_pso_config_init() first
	if (ctx == NULL) return;
    ctx->config.layout.side_len_pt 			= 		PSO_SIDE_LEN_PT;
    ctx->config.layout.grid_spacing 		= 		(fabs(ctx->problem->upper[0] - ctx->problem->lower[0]))/ (ctx->config.layout.side_len_pt - 1);
    ctx->config.layout.fixed_particles 		= 		ctx->config.layout.side_len_pt * ctx->config.layout.side_len_pt;
	ctx->config.layout.shift_side_len_half 	= 		((ctx->config.layout.side_len_pt-1)/2) * ctx->config.layout.grid_spacing;
}

int st_pso_memory_alloc(struct pso_context *ctx) {
	ctx->particle = malloc(sizeof(struct pso_particle)*ctx->config.population);
	if (!ctx->particle) { 
		DBG_MSG("malloc particle failed!!\n");
		return -1;
	}
	return 0;
}

struct pso_algorithm_ops standard_pso_algo_ops = {
    .init           = 	st_pso_algo_init,
    .run  			= 	st_pso_algo_run,
	.cleanup		= 	st_pso_algo_cleanup
};

int st_pso_algo_init(struct pso_context *ctx) {
	DBG_MSG("st_pso_algo_init\n");
    if (ctx == NULL) return PSO_FAILURE;
	st_pso_config_init(ctx);
	st_pso_layout_init(ctx);
	if (st_pso_memory_alloc(ctx) != 0) {
		DBG_MSG("[ERROR] Allocating memory failed!!\n");
		return PSO_FAILURE;
	}
	if (ctx->config.population < ctx->config.layout.fixed_particles) {
		goto init_err;
	}
	return PSO_SUCCESS;

init_err:
	DBG_MSG("[ERROR] population < fixed_particles!\n");
	st_pso_algo_cleanup(ctx);
	return PSO_FAILURE;
}

static inline int is_better(double candidate, double current, optim_direction_t direction) {
	return (direction == OPTIM_MAXIMIZE && candidate > current) ||
			(direction == OPTIM_MINIMIZE && candidate < current);
}

int st_pso_algo_run(struct pso_context *ctx) {
	DBG_MSG("st_pso_algo_run\n");
	if (ctx == NULL) return -1;
	double c1 								= ctx->config.c1;
	double c2 								= ctx->config.c2;
	struct pso_particle *particles 			= ctx->particle;
	double spacing 							= ctx->config.layout.grid_spacing;
	double half_len 						= ctx->config.layout.shift_side_len_half;
	init_random_seed(); // initialize random var (only do once)
	// initialize fixed points
	for (int p_idx = 0; p_idx < ctx->config.population; p_idx++) {
		if (p_idx < (ctx->config.layout.fixed_particles)) {
			double x_pos = (double)(p_idx % ctx->config.layout.side_len_pt) * spacing - (double)half_len;
			double y_pos = (double)(p_idx / ctx->config.layout.side_len_pt) * spacing - (double)half_len;
			particles[p_idx].position[1] = x_pos;
			particles[p_idx].position[0] = y_pos;
			for (int dim=0; dim<ctx->config.dimension; dim++) {
				particles[p_idx].velocity[dim] = ctx->config.v_ini*(rand0_1());
				particles[p_idx].ind_best_pos[dim] = particles[p_idx].position[dim];
			}
			double cost = ctx->problem->evaluate(ctx->problem->data, particles[p_idx].position);
			DBG_MSG("cost = %.4f\n", cost);
			particles[p_idx].ind_best_value = cost;
			if (is_better(cost, ctx->global_best.value, ctx->problem->direction)) {
				ctx->global_best.value = cost;
				ctx->global_best.index = p_idx;
				for (int dim=0; dim<ctx->config.dimension; dim++) {
					ctx->global_best.position[dim] = particles[p_idx].position[dim];
				}
			}
		} 
		// initialize random points
		else {
			for (int dim=0; dim<ctx->config.dimension; dim++) {
				particles[p_idx].velocity[dim] = ctx->config.v_ini*(rand0_1()); // should initialize in .init
				double low = ctx->problem->lower[dim];
				double high = ctx->problem->upper[dim];
				particles[p_idx].position[dim] = low + (high - low) * rand0_1();
				particles[p_idx].ind_best_pos[dim] = particles[p_idx].position[dim];
			}
			double cost = ctx->problem->evaluate(ctx->problem->data, particles[p_idx].position);
			particles[p_idx].ind_best_value = cost;
			if (is_better(cost, ctx->global_best.value, ctx->problem->direction)) {
				ctx->global_best.value = cost;
				ctx->global_best.index = p_idx;
				for (int dim=0; dim<ctx->config.dimension; dim++) {
					ctx->global_best.position[dim] = particles[p_idx].position[dim];
				}
			}
		}
	}
	// iteration start
	for (int iter = 0; iter < ctx->config.max_iter; iter++) {
		for (int p_idx = 0; p_idx < ctx->config.population; p_idx++) {
			for (int dim = 0; dim<ctx->config.dimension; dim++) {
				double weight = ctx->config.w_max - ((double)(iter + 1) / ctx->config.max_iter)*(ctx->config.w_max - ctx->config.w_min);
				particles[p_idx].velocity[dim] = weight * particles[p_idx].velocity[dim] +\
												 c1 * (rand0_1()) * (particles[p_idx].ind_best_pos[dim] - particles[p_idx].position[dim]) +\
												 c2 * (rand0_1()) * (ctx->global_best.position[dim] - particles[p_idx].position[dim]);

				clamp_to_bounds(&particles[p_idx].velocity[dim], ctx->config.v_max, ctx->config.v_min);
				particles[p_idx].position[dim] += particles[p_idx].velocity[dim];
				clamp_to_bounds(&particles[p_idx].position[dim], ctx->problem->upper[dim], ctx->problem->lower[dim]);
			}
			double cost = ctx->problem->evaluate(ctx->problem->data, particles[p_idx].position);
			if (is_better(cost, particles[p_idx].ind_best_value, ctx->problem->direction)) {
				particles[p_idx].ind_best_value = cost;
				for (int dim = 0; dim<ctx->config.dimension; dim++) {
					particles[p_idx].ind_best_pos[dim] = particles[p_idx].position[dim];
				}
				if (is_better(cost, ctx->global_best.value, ctx->problem->direction)) {
					ctx->global_best.value = cost;
					ctx->global_best.index = p_idx;
					for (int dim = 0; dim<ctx->config.dimension; dim++) {
						ctx->global_best.position[dim] = particles[p_idx].position[dim];
					}
				}
			}
		}
	}
	// Result: ctx->global_best
	DBG_MSG("[INFO] PSO finished!\n");
	return PSO_SUCCESS;
}

int st_pso_algo_cleanup(struct pso_context *ctx) {
	DBG_MSG("st_pso_algo_cleanup\n");
	if (ctx == NULL) return -1;
	if (ctx->particle) {
		free(ctx->particle);
		ctx->particle = NULL;
	}
	return PSO_SUCCESS;
}

int main(int argc, char *argv[]) {
    return 0;
}