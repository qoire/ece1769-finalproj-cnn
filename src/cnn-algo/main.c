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

void conv(const float *I, int M, int N, int D, const float *W, int Mf, int Nf, int Df, const float *B, float *O)
{
    int M0 = M - Mf + 1;
    int N0 = N - Nf + 1;
    for (int d0 = 0; d0 < Df; d0++)
        for (int y0 = 0; y0 < N0; y0++)
            for (int x0 = 0; x0 < M0; x0++) {
                float sum = 0;
                for (int d = 0; d < D; d++)
                    for (int y = 0; y < Nf; y++)
                        for (int x = 0; x < Mf; x++)
                            sum += I[d*M*N + (y + y0)*M + x + x0] * W[d0*Mf*Nf*D + d*Mf*Nf + y*Mf + x];
                sum += B[d0];
                O[d0*M0*N0 + y0*M0 + x0] = (float)sum;
            }
}

void pool(float *I, int M, int N, int D, int stride, float *O)
{
    int M0 = M / stride;
    int N0 = N / stride;
    for (int d0 = 0; d0 < D; d0++)
        for (int y0 = 0; y0 < N; y0 += stride)
            for (int x0 = 0; x0 < M; x0 += stride) {
                float max = I[d0*M*N + y0*M + x0];
                for (int y = 0; y < stride; y++)
                    for (int x = 0; x < stride; x++) {
                        float v = I[d0*M*N + (y0 + y)*M + x0 + x];
                        if (v > max)
                            max = v;
                    }
                O[d0*M0*N0 + (y0/stride)*M0 + x0/stride] = max;
            }
}

void relu(float *I, int M, int N, int D, float *O)
{
    for (int d0 = 0; d0 < D; d0++)
        for (int y0 = 0; y0 < N; y0++)
            for (int x0 = 0; x0 < M; x0++)
                O[d0*M*N + y0*M + x0] = I[d0*M*N + y0*M + x0] > 0 ? I[d0*M*N + y0*M + x0] : 0;
}

int simplenn(const float *image) {
    float layer1[24*24*20];
    float layer2[12*12*20];
    float layer3[8*8*50];
    float layer4[4*4*50];
    float layer5[1*500];
    float layer6[1*500];
    float layer7[1*10];

    conv(image,  28, 28,   1, layer1_weights, 5, 5,  20, layer1_bias, layer1);
    pool(layer1, 24, 24,  20, 2, layer2);
    conv(layer2, 12, 12,  20, layer3_weights, 5, 5,  50, layer3_bias, layer3);
    pool(layer3,  8,  8,  50, 2, layer4);
    conv(layer4,  4,  4,  50, layer5_weights, 4, 4, 500, layer5_bias, layer5);
    relu(layer5,  1,  1, 500, layer6);
    conv(layer6,  1,  1, 500, layer7_weights, 1, 1,  10, layer7_bias, layer7);

    float max = layer7[0];
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
    clock_t start, end;

    clock_t per_cycle_start, per_cycle_end;

    start = clock();

    for (int i = 0; i < 10000; i++) {
        //per_cycle_start = clock();
        if (simplenn(images[i]) != labels[i] - 1) {
//            printf("Bad at %d\n", i + 1);
            bad++;
        }

        //per_cycle_end = clock();
    }

    end = clock();

    printf("Total bad: %d\n", bad);

    printf("Time taken %lf\n", (double) (end-start) / CLOCKS_PER_SEC);

    return 0;
}
