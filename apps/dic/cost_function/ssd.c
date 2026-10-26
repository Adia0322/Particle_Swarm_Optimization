#include <stdlib.h>
#include <math.h>
#include <pso_math.h>
#include "dic_comm.h"
#include "pso_comm.h"
#include "interp.h"
#include "optim_problem.h"

// SSD Critiria
struct pso_cost_func_ops ssd_ops = {
	.init			= ssd_init,
    .cost_function  = ssd_cost_function,
	.cleanup		= ssd_cleanup
};

struct dic_ssd_context *ssd_create(const struct dic_input_data *input) {
	return NULL;
}

int ssd_init(void *priv) {
	return 0;
}

double ssd_cost_function(void *priv, const double *position) {
	return 0;
}

int ssd_cleanup(void *priv) {
	return 0;
}
