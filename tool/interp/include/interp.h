#ifndef interp_comm_H
#define interp_comm_H

#include <stdio.h>
#include <stdint.h>

int interp_safe_margin();
double bilinear(double *data, int width, int height, double x, double y);

#endif

