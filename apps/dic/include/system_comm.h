#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "dic_comm.h"
#include "pso_comm.h"

#define SYS_FAILURE              				-1
#define SYS_SUCCESS               				0
// #define SYS_DBG_EN                				1
#define SYS_ERR_EN				  				1

#define SQUARE(x) ((x)*(x))
#define MEAN(x,y) ((x)/((y)*(y)))

#define IMG_WIDTH_640                           640
#define IMG_HEIGHT_480                          480 
#define IMG_WIDTH_1280                          1280
#define IMG_HEIGHT_720                          720 
#define IMG_WIDTH_1920                          1920
#define IMG_HEIGHT_1080                         1080 


#if defined(SYS_DBG_EN)
#define SYS_DBG               SYS_PRT
#else
#define SYS_DBG(format, args...)	do {} while (0) // erase: SYS_DBG(<all things you input>)
#endif

#define SYS_PRT(format, ...)	printf("[%s][%d]"format, __func__, __LINE__, ##__VA_ARGS__)
// args... similar to ...

#if defined(SYS_ERR_EN)
#define SYS_ERR					SYS_PRT
#else
#define SYS_ERR(format, ...)	do {} while (0)
#endif

typedef enum {
    cost_func_zncc,
    cost_func_ssd
} pso_cost_type;

int dic_cost_setup(struct pso_context *pso_ctx, struct dic_input_data *input, pso_cost_type type);
void init_random_seed();
double random_digit(void);
double get_mean(double sum, int side_len);

#endif
