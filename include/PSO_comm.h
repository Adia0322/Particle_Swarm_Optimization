#ifndef PSO_comm_H
#define PSO_comm_H

#include <stdio.h>
#include <stdint.h>

#define PSO_FAILURE                 -1
#define PSO_SUCCESS                 0

#define DEFAULT_POPULATION          50
#define PSO_CFG_POPULATION          DEFAULT_POPULATION
#define PSO_CFG_DIMENSION           2
#define PSO_CFG_MAX_ITER            10
#define PSO_CFG_V_MAX               4
#define PSO_CFG_V_MIN               -4
#define PSO_CFG_V_INI               1
#define PSO_CFG_W_MAX               0.9f
#define PSO_CFG_W_MIN               0.4f
#define PSO_CFG_C1                  2.0f
#define PSO_CFG_C2                  2.0f
#define PSO_CFG_DEC_FACTOR          1.0f
#define PSO_CFG_INC_FACTOR          1.0f
#define PSO_CFG_BORDER_LEN          1
#define PSO_CFG_BORDER_SCALE        0.05

#define PSO_SIDE_LEN_PT             5

typedef struct pso_context pso_context;
typedef struct sys_info sys_info;
typedef struct pso_context pso_context;

typedef enum {
    PSO_MINIMIZE,
    PSO_MAXIMIZE
} pso_direction;

typedef enum {
    VELOCITY,
	POSITION,
} pso_bound_check_type;

typedef enum {
    cost_func_zncc,
    cost_func_ssd
} pso_cost_type;

struct pso_global_best {
    double position[2]; // x, y
    double value;
    int index;
};

struct pso_layout_config{
    int side_len_pt;                   // matrix_side_len_points
    double grid_spacing;               // matrix_interval_pixel
    int fixed_particles;               // inital all fixed particles
    int shift_side_len_half;           // correct the y corridate of point
};

struct pso_config{
    struct pso_layout_config layout;
    int population;
    int dimension;
    int max_iter;
    // double iteration_rec; 
    int v_max;                         // velocity_max
    int v_min;                         // velocity_min
    int v_ini;                         // velocity_initial
    double w_max;                      // w_upper
    double w_min;                      // w_lower
    double c1;                         // cognition_factor
    double c2;                         // social_factor
    double dec_factor;                 // Decrease_factor
    double inc_factor;                 // Increase_factor
    double border_side_len;            // border length 
};

struct pso_particle {
    double position[2];
    double velocity[2];
    double ind_best_pos[2];
    double ind_best_value;
};

typedef struct pso_context {
    struct pso_config config;
    struct pso_algorithm_ops *algo_ops;     // 與priv不同 填入特定的ops: 表示:怎麼做 (指標指向靜態Struct)
    struct pso_cost_func_ops *cost_ops;
    struct pso_particle *particle;
    struct pso_global_best global_best; 
    void *priv;                             // private 為特定任務中所需的"資料" 表示:用什麼做
}pso_context;

struct pso_algorithm_ops {
    int (*init)(pso_context *ctx);
    int (*run)(pso_context *ctx);
    int (*cleanup)(pso_context *ctx);
};

struct pso_cost_func_ops {
    int (*init)(sys_info *info);
    double (*cost_function)(pso_context *ctx, const double *position);
    int (*cleanup)(pso_context *ctx);
};

void run_pso(struct sys_info *info, pso_cost_type type);
void pso_create_factory(struct sys_info *info, pso_cost_type type);
int pso_destroy(struct pso_context *pso_ctx);
int execute_pso_algorithm(struct pso_context *ctx);
int st_pso_algo_init(struct pso_context *ctx);
int st_pso_algo_run(struct pso_context *ctx);
int st_pso_algo_cleanup(struct pso_context *ctx);
double double_rand_nor(void);

extern struct pso_cost_func_ops zncc_ops;
extern struct pso_cost_func_ops ssd_ops;
extern struct pso_algorithm_ops standard_pso_algo_ops;

#endif

