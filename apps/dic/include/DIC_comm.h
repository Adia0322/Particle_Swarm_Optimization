#ifndef DIC_COMM_H
#define DIC_COMM_H

#include <stdint.h>
#include <stdlib.h>
#include "pso_comm.h"

typedef enum {
    cost_func_zncc,
    cost_func_ssd
} pso_cost_type;

typedef struct dic_config {
    uint16_t subset_side_len;
    uint16_t scan_side_len;
} dic_config;

typedef struct img_info{
    double *ref_data;
    double *cur_data;
    uint32_t width;
    uint32_t height;
} img_info;

typedef struct dic_input_data {
	double *ref_img;
    double *cur_img;
    double *img_ref_pt;
    double *img_cur_pt;
    double *result;
    int width;
    int height;
    int population;
    int subset_side_len;
} dic_input_data;

typedef struct dic_context {
	double *ref_img;
    double *cur_img;
    double *img_ref_pt;
    double *img_cur_pt;
    double *result;
    int width;
    int height;
    int population;
    int subset_side_len;
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
    int subset_side_len;
    int margin;
} dic_zncc_context;

typedef struct dic_ssd_context {
    struct dic_subset_info ref_subset_info;
    struct dic_subset_info cur_subset_info;
    struct img_info img_info;
    double *temp_subset;
    double *ref_subset_zero_mean;
    double subset_pt_cur_pos[2];
    double img_pt_ref_pos[2];
    double img_pt_cur_pos[2];
    int margin;
} dis_ssd_context;

double get_subset_mean(const dic_subset_info *subset);
void zncc_configure(struct dic_zncc_context *ctx, struct dic_context *dic_ctx);
int zncc_alloc_buffers(struct dic_zncc_context *zncc_ctx);
int zncc_init(void *priv);
double zncc_cost_function(void *priv, const double *position);
int zncc_cleanup(void *priv);
int ssd_init(void *priv);
double ssd_cost_function(void *priv, const double *position);
int ssd_cleanup(void *priv);
struct dic_zncc_context *zncc_create(struct dic_input_data *input);
struct dic_ssd_context *ssd_create(struct dic_input_data *input);

extern struct pso_cost_func_ops zncc_ops;
extern struct pso_cost_func_ops ssd_ops;

#endif
