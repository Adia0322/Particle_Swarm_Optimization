#ifndef IMAGE_COMM_H
#define IMAGE_COMM_H

#include <stdlib.h>
#include <stdint.h>

typedef struct img_info{
    double *ref_data;
    double *cur_data;
    uint32_t width;
    uint32_t height;
    uint32_t pixel_format;
} img_info;

#endif