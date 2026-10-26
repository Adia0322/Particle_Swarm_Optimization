#include <stdlib.h>
#include <math.h>
#include <pso_math.h>
#include "dic_comm.h"
#include "pso_comm.h"
#include "interp.h"
#include "optim_problem.h"

double inline get_subset_mean(const dic_subset_info *subset) {
	if (!subset) return NAN;
    double sum = 0.0f;
	int num = subset->total_pixels;
    const double *data = subset->subset_data;
    for (int i = 0; i < num; i++) {
        sum += data[i];
    }
    return sum / (double)num;
}

void zncc_precompute_ref_sum_den(struct dic_zncc_context *zncc_ctx) {
	double *ref_subset_data 			= zncc_ctx->ref_subset_info.subset_data;
	double *ref_subset_zero_mean 		= zncc_ctx->ref_subset_zero_mean;
	double ref_subset_mean 				= zncc_ctx->ref_subset_info.mean;
	int subset_side_len 				= zncc_ctx->ref_subset_info.side_len;
	double ref_sum_den 					= 0.0f;
	for (int row = 0; row < subset_side_len; row++) {
		for (int col = 0; col < subset_side_len; col++) {
			double mean_substrat_ref_subset = ((ref_subset_data[row * subset_side_len + col]) - ref_subset_mean);
			ref_subset_zero_mean[row * subset_side_len + col] = mean_substrat_ref_subset;
			ref_sum_den += SQUARE(mean_substrat_ref_subset);
		}
	}
	zncc_ctx->ref_subset_info.ref_sum_den = ref_sum_den;
}

static inline void extract_subset_bilinear(const double *img, int w, int h, 
                                           double start_x, double start_y, 
                                           int side_len, double *temp_subset) {
    int x0_base = (int)start_x;
    int y0_base = (int)start_y;

    double dx = start_x - x0_base;
    double dy = start_y - y0_base;

    double w00 = (1.0f - dx) * (1.0f - dy);
    double w01 = dx * (1.0f - dy);
    double w10 = (1.0f - dx) * dy;
    double w11 = dx * dy;

    for (int row = 0; row < side_len; row++) {

        int row0 = (y0_base + row) * w;
        int row1 = row0 + w;

        int shift = row * side_len;

        for (int col = 0; col < side_len; col++) {

            int x0 = x0_base + col;

            double v00 = img[row0 + x0];
            double v01 = img[row0 + x0 + 1];
            double v10 = img[row1 + x0];
            double v11 = img[row1 + x0 + 1];

            temp_subset[shift + col] =
                w00 * v00 +
                w01 * v01 +
                w10 * v10 +
                w11 * v11;
        }
    }
}

static void zncc_get_ref_subset_data(struct dic_zncc_context *zncc_ctx) {
	double *pos_ptr 					= zncc_ctx->img_pt_ref_pos;
	double *ref_img_data 				= zncc_ctx->img_info.ref_data;
	int subset_side_len					= zncc_ctx->ref_subset_info.side_len;
	int img_width 						= zncc_ctx->img_info.width;
	int img_height 						= zncc_ctx->img_info.height;
	double img_pt_y_shift 				= pos_ptr[0] - (subset_side_len - 1) / 2.0f;
	double img_pt_x_shift 				= pos_ptr[1] - (subset_side_len - 1) / 2.0f;
	double *ref_subset_data 			= zncc_ctx->ref_subset_info.subset_data;
	for (int row = 0; row < subset_side_len; row++) {
		for (int col = 0; col < subset_side_len; col++) {
			double img_pt_y_shift_row = img_pt_y_shift + row;
			double img_pt_x_shift_col = img_pt_x_shift + col;
			int pixel_idx = row * subset_side_len + col;
			ref_subset_data[pixel_idx] = bilinear(ref_img_data, img_width, img_height,
												img_pt_x_shift_col, img_pt_y_shift_row);
		}
	}
	zncc_ctx->ref_subset_info.mean = get_subset_mean(&zncc_ctx->ref_subset_info);
}

struct dic_zncc_context *zncc_ctx_create(const struct dic_input_data *input) {
	if (!input) return NULL;
	struct dic_zncc_context *ctx = malloc(sizeof(struct dic_zncc_context));
	if (!ctx) {
		DBG_MSG("Malloc dic_zncc_context failed!!\n");
		return NULL;
	}
	*ctx = (struct dic_zncc_context){0};
	ctx->img_info.ref_data 					= input->ref_img;
    ctx->img_info.cur_data 					= input->cur_img;
	ctx->img_pt_ref_pos[0]				    = input->img_ref_pt[0];
	ctx->img_pt_ref_pos[1]				    = input->img_ref_pt[1];
	ctx->img_pt_cur_pos[0]				    = input->img_cur_pt[0];
	ctx->img_pt_cur_pos[1]				    = input->img_cur_pt[1];
	ctx->img_info.width				    	= input->width;
	ctx->img_info.height				    = input->height;
	ctx->ref_subset_info.side_len 		    = input->subset_side_len;
	ctx->cur_subset_info.side_len 		    = input->subset_side_len;

	ctx->cur_subset_info.subset_data 	    = NULL; // calculate in runtime		
	ctx->ref_subset_info.total_pixels 	    = SQUARE(ctx->ref_subset_info.side_len);
	ctx->cur_subset_info.total_pixels 	    = SQUARE(ctx->cur_subset_info.side_len);
	ctx->margin							    = interp_safe_margin();

	zncc_init(ctx);
	return ctx;
}

int zncc_alloc_buffers(struct dic_zncc_context *zncc_ctx) {
	int subset_memory_size = SQUARE(zncc_ctx->cur_subset_info.side_len) * sizeof(double);
	zncc_ctx->cur_subset_info.subset_data 	= malloc(subset_memory_size);
	zncc_ctx->ref_subset_info.subset_data 	= malloc(subset_memory_size);
	zncc_ctx->temp_subset 				  	= malloc(subset_memory_size);
	zncc_ctx->ref_subset_zero_mean		  	= malloc(subset_memory_size);
	if (!zncc_ctx->cur_subset_info.subset_data ||
		!zncc_ctx->ref_subset_info.subset_data ||
		!zncc_ctx->temp_subset ||
		!zncc_ctx->ref_subset_zero_mean) { 
		goto error_free_buffers;
	}
	return 0;

error_free_buffers:
	DBG_MSG("Malloc subset_data failed!!\n");
	free(zncc_ctx->cur_subset_info.subset_data);
	free(zncc_ctx->ref_subset_info.subset_data);
	free(zncc_ctx->temp_subset);
	free(zncc_ctx->ref_subset_zero_mean);
	zncc_ctx->ref_subset_zero_mean = NULL;
	zncc_ctx->temp_subset = NULL;
	zncc_ctx->ref_subset_info.subset_data = NULL;
	zncc_ctx->cur_subset_info.subset_data = NULL;
	return -1;
}

struct pso_cost_func_ops zncc_ops = {
	.init			= zncc_init,
    .cost_function  = zncc_cost_function,
	.cleanup		= zncc_cleanup
};

int zncc_init(void *priv) {
	DBG_MSG("zncc_init\n");
	if (!priv) return -1;
	struct dic_zncc_context *ctx = priv;

	if (zncc_alloc_buffers(ctx) != 0) {
		DBG_MSG("Allocating memory failed!!\n");
		return -1;
	}
	zncc_get_ref_subset_data(ctx);
	zncc_precompute_ref_sum_den(ctx);
	return 0;
}

double zncc_cost_function(void *priv, const double *position) {
	// DBG_MSG("zncc_cost_function\n");
	if (priv == NULL || position == NULL) return NAN;
	struct dic_zncc_context *zncc_ctx	= priv;
	double rel_pso_pt_y 				= position[0]; 	// Pi_x, Pi_y relative corrected coordinate
	double rel_pso_pt_x 				= position[1]; 	// Pi_x, Pi_y relative corrected coordinate
	double img_cur_pt_y 				= zncc_ctx->img_pt_cur_pos[0]; 		// start point (fixed in whole PSO process)
	double img_cur_pt_x 				= zncc_ctx->img_pt_cur_pos[1]; 		// start point (fixed in whole PSO process)
	int img_width 						= zncc_ctx->img_info.width;
	int img_height 						= zncc_ctx->img_info.height;
	double *cur_img_data 				= zncc_ctx->img_info.cur_data;
	int subset_side_len 				= zncc_ctx->ref_subset_info.side_len;
	double *ref_subset_data 			= zncc_ctx->ref_subset_info.subset_data;
	double img_pso_pt_y 				= img_cur_pt_y + rel_pso_pt_y; 		// pso particle absolute coordinate
	double img_pso_pt_x 				= img_cur_pt_x + rel_pso_pt_x; 		// pso particle absolute coordinate
	double img_pso_pt_y_shift 			= img_pso_pt_y - ((subset_side_len - 1) / 2.0f); // shift to left_top_point, means (0,0) point in subset matrix(n,n)
	double img_pso_pt_x_shift			= img_pso_pt_x - ((subset_side_len - 1) / 2.0f);
	int margin							= zncc_ctx->margin;

	if ((img_pso_pt_y_shift + subset_side_len + margin) > img_height || (img_pso_pt_y_shift + margin) < 0) {
		DBG_MSG("shifted PSO particle image pos_y out of bound!\n");
		DBG_MSG("img_pso_pt_y_shift: %.2f\n", img_pso_pt_y_shift);
		return NAN; // result range: -1 ~ +1
	} else if ((img_pso_pt_x_shift + subset_side_len + margin) > img_width || (img_pso_pt_x_shift + margin) < 0) {
		DBG_MSG("shifted PSO particle image pos_x out of bound!\n");
		DBG_MSG("img_pso_pt_x_shift: %.2f\n", img_pso_pt_x_shift);
		return NAN; // result range: -1 ~ +1
	}
	
	if (!ref_subset_data) {
		DBG_MSG("Error: ref_subset_data is NULL!\n");
		return NAN;
	}

	double ref_sum_den = zncc_ctx->ref_subset_info.ref_sum_den;
    if (ref_sum_den <= 0.0) return NAN;

	extract_subset_bilinear(cur_img_data, img_width, img_height, 
                            img_pso_pt_x_shift, img_pso_pt_y_shift, 
                            subset_side_len, zncc_ctx->temp_subset);
	
	double * restrict g   	= zncc_ctx->temp_subset;
	double * restrict f_zm  = zncc_ctx->ref_subset_zero_mean;

	int pixels 				= zncc_ctx->ref_subset_info.total_pixels;
    
	double sum_g = 0.0, sum_fg = 0.0, sum_gg = 0.0;
    for (int i = 0; i < pixels; i++) {
        double gi = g[i];
        double fi_zm = f_zm[i];
        sum_g += gi;
        sum_fg += fi_zm * gi;
        sum_gg += gi * gi;
    }
	double g_mean   		= sum_g / pixels;
	double numer    		= sum_fg;
	double cur_den  		= sum_gg  - pixels * g_mean * g_mean;

	if (cur_den <= 0.0) return NAN;
    return numer / sqrt(ref_sum_den * cur_den);
}

int zncc_cleanup(void *priv) {
	DBG_MSG("zncc_cleanup\n");
	if (priv == NULL) {
		DBG_MSG("zncc_cleanup: priv == NULL\n");
		return -1;
	}
	struct dic_zncc_context *ctx = priv;
    free(ctx->cur_subset_info.subset_data);
    free(ctx->ref_subset_info.subset_data);
    free(ctx->temp_subset);
    free(ctx->ref_subset_zero_mean);
    free(ctx);

    DBG_MSG("zncc_cleanup: Resources released\n");
	return 0;
}

