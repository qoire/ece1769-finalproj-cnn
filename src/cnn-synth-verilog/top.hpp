#ifndef TOP_H
#define TOP_H

#include "layer-weights-bias.h"
#include "ap_int.h"

#define FIX_POINT_SCALAR 256
#define FIX_POINT_SHIFT 8

void top(const int *image, int *ret);

#endif
