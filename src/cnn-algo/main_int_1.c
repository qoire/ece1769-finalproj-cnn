#include <stdio.h>
#include <time.h>

#include "layer1-weights.h"
#include "layer1-bias.h"
#include "layer3-weights.h"
#include "layer3-bias.h"
#include "layer5-weights.h"
#include "layer5-bias.h"
#include "layer7-weights.h"
#include "layer7-bias.h"

#include "images.h"

#define FIX_POINT_SCALAR 256
#define FIX_POINT_SHIFT 8

void conv(const int *I, int M, int N, int D, const int *W, int Mf, int Nf, int Df, const int *B, int *O)
{
    int M0 = M - Mf + 1;
    int N0 = N - Nf + 1;
    for (int d0 = 0; d0 < Df; d0++)
        for (int y0 = 0; y0 < N0; y0++)
            for (int x0 = 0; x0 < M0; x0++) {
                int sum = 0;
                for (int d = 0; d < D; d++)
                    for (int y = 0; y < Nf; y++)
                        for (int x = 0; x < Mf; x++)
                            sum += ((I[d*M*N + (y + y0)*M + x + x0] * W[d0*Mf*Nf*D + d*Mf*Nf + y*Mf + x]) >> FIX_POINT_SHIFT);
                sum += B[d0];
                O[d0*M0*N0 + y0*M0 + x0] = sum;
            }
}

void pool(int *I, int M, int N, int D, int stride, int *O)
{
    int M0 = M / stride;
    int N0 = N / stride;
    for (int d0 = 0; d0 < D; d0++)
        for (int y0 = 0; y0 < N; y0 += stride)
            for (int x0 = 0; x0 < M; x0 += stride) {
                int max = I[d0*M*N + y0*M + x0];
                for (int y = 0; y < stride; y++)
                    for (int x = 0; x < stride; x++) {
                        int v = I[d0*M*N + (y0 + y)*M + x0 + x];
                        if (v > max)
                            max = v;
                    }
                O[d0*M0*N0 + (y0/stride)*M0 + x0/stride] = max;
            }
}

void relu(int *I, int M, int N, int D, int *O)
{
    for (int d0 = 0; d0 < D; d0++)
        for (int y0 = 0; y0 < N; y0++)
            for (int x0 = 0; x0 < M; x0++)
                O[d0*M*N + y0*M + x0] = I[d0*M*N + y0*M + x0] > 0 ? I[d0*M*N + y0*M + x0] : 0;
}

int simplenn(const int *image, int *W1, int *B1, int *W3, int *B3, int *W5, int *B5, int *W7, int *B7) {
    int layer1[24*24*20];
    int layer2[12*12*20];
    int layer3[8*8*50];
    int layer4[4*4*50];
    int layer5[1*500];
    int layer6[1*500];
    int layer7[1*10];

    conv(image,  28, 28,   1, W1, 5, 5,  20, B1, layer1);
    pool(layer1, 24, 24,  20, 2, layer2);
    conv(layer2, 12, 12,  20, W3, 5, 5,  50, B3, layer3);
    pool(layer3,  8,  8,  50, 2, layer4);
    conv(layer4,  4,  4,  50, W5, 4, 4, 500, B5, layer5);
    relu(layer5,  1,  1, 500, layer6);
    conv(layer6,  1,  1, 500, W7, 1, 1,  10, B7, layer7);

    int max = layer7[0];
    int n = 0;
    for (int i = 1; i < 10; i++)
        if (layer7[i] > max) {
            max = layer7[i];
            n = i;
    }

    return n;
}

int main()
{
    int bad = 0;
    int layer1_weights_int[5*5*1*20];
    int layer1_bias_int[20];
    int layer3_weights_int[5*5*20*50];
    int layer3_bias_int[50];
    int layer5_weights_int[4*4*50*500];
    int layer5_bias_int[500];
    int layer7_weights_int[1*1*500*10];
    int layer7_bias_int[10];
    int image[28*28];
    clock_t start, end;


    for (int i = 0; i < 5*5*1*20; i++)
        layer1_weights_int[i] = (int)(layer1_weights[i] * FIX_POINT_SCALAR);
    for (int i = 0; i < 20; i++)
        layer1_bias_int[i] = (int)(layer1_bias[i] * FIX_POINT_SCALAR);
    for (int i = 0; i < 5*5*20*50; i++)
        layer3_weights_int[i] = (int)(layer3_weights[i] * FIX_POINT_SCALAR);
    for (int i = 0; i < 50; i++)
        layer3_bias_int[i] = (int)(layer3_bias[i] * FIX_POINT_SCALAR);
    for (int i = 0; i < 4*4*50*500; i++)
        layer5_weights_int[i] = (int)(layer5_weights[i] * FIX_POINT_SCALAR);
    for (int i = 0; i < 500; i++)
        layer5_bias_int[i] = (int)(layer5_bias[i] * FIX_POINT_SCALAR);
    for (int i = 0; i < 1*1*500*10; i++)
        layer7_weights_int[i] = (int)(layer7_weights[i] * FIX_POINT_SCALAR);
    for (int i = 0; i < 10; i++)
        layer7_bias_int[i] = (int)(layer7_bias[i] * FIX_POINT_SCALAR);

    start = clock();

    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 28*28; j++)
            image[j] = (int)(images[i][j] * FIX_POINT_SCALAR);
        if (simplenn(image, layer1_weights_int, layer1_bias_int,
                            layer3_weights_int, layer3_bias_int,
                            layer5_weights_int, layer5_bias_int,
                            layer7_weights_int, layer7_bias_int) != labels[i] - 1) {
            printf("Bad at %d\n", i + 1);
            bad++;
        }
    }

    end = clock();

    printf("Total bad: %d\n", bad);

    printf("Time taken %lf\n", (double) (end-start) / CLOCKS_PER_SEC);

    return 0;
}
