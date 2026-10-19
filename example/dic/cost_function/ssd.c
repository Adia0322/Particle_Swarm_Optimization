#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "system_comm.h"
#include "DIC_comm.h"
#include "PSO_comm.h"

// SSD Critiria
struct pso_cost_func_ops ssd_ops = {
	.init			= ssd_init,
    .cost_function  = ssd_cost_function,
	.cleanup		= ssd_cleanup
};

int ssd_init(struct sys_info *info) {
	return 0;
}

double ssd_cost_function(struct pso_context *ctx, const double *position) {
	return 0;
}

int ssd_cleanup(struct pso_context *ctx) {
	return 0;
}