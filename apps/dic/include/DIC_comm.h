#ifndef DIC_COMM_H
#define DIC_COMM_H

#include <stdint.h>
#include <stdlib.h>
#include "pso_comm.h"
#include "image_comm.h"

typedef struct dic_config {
    uint16_t subset_side_len;
    uint16_t scan_side_len;
} dic_config;

typedef struct dic_context {
    struct dic_config config;
    struct img_info img_info;
    double *img_ref_pt_pos;
    double *img_cur_pt_pos;
    void *priv;
} dic_context;

typedef struct dic_subset_info {
    double *subset_data;
    double mean;
    double ref_sum_den;
    int side_len;
    int total_pixels;
} dic_subset_info;

typedef struct dic_zncc_context {
    struct dic_subset_info ref_subset_info;
    struct dic_subset_info cur_subset_info;
    struct img_info img_info;
    double *temp_subset;
    double *ref_subset_zero_mean;
    double subset_pt_cur_pos[2];
    double img_pt_ref_pos[2];
    double img_pt_cur_pos[2];
    int margin;
} dic_zncc_context;

typedef struct dis_ssd_context {
    struct dic_subset_info ref_subset_info;
    struct dic_subset_info cur_subset_info;
    double subset_pt_cur_pos[2];
    double img_pt_ref_pos[2];
} dis_ssd_context;

double get_subset_mean(const dic_subset_info *subset);
void zncc_configure(struct dic_zncc_context *ctx, struct dic_context *dic_ctx);
int zncc_alloc_buffers(struct dic_zncc_context *zncc_ctx);
int zncc_init(struct sys_info *info);
double zncc_cost_function(struct pso_context *ctx, const double *position);
int zncc_cleanup(struct pso_context *ctx);
int ssd_init(struct sys_info *info);
double ssd_cost_function(struct pso_context *ctx, const double *position);
int ssd_cleanup(struct pso_context *ctx);

extern struct pso_cost_func_ops zncc_ops;
extern struct pso_cost_func_ops ssd_ops;

#endif
