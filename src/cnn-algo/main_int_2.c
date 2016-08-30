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


static int W1[5*5*1*20];
static int B1[20];
static int W3[5*5*20*50];
static int B3[50];
static int W5[4*4*50*500];
static int B5[500];
static int W7[1*1*500*10];
static int B7[10];

static int I[28*28];
static int L1[24*24*20];
static int L2[12*12*20];
static int L3[8*8*50];
static int L4[4*4*50];
static int L5[1*500];
static int L6[1*500];
static int L7[1*10];


void conv1()
{
    int M = 28, N = 28, D = 1;
    int Mf = 5, Nf = 5, Df = 20;
    int M0 = M - Mf + 1;
    int N0 = N - Nf + 1;
    for (int d0 = 0; d0 < Df; d0++)
        for (int y0 = 0; y0 < N0; y0++)
            for (int x0 = 0; x0 < M0; x0++) {
                int sum = 0;
                for (int d = 0; d < D; d++)
                    for (int y = 0; y < Nf; y++)
                        for (int x = 0; x < Mf; x++)
                            sum += ((I[d*M*N + (y + y0)*M + x + x0] * W1[d0*Mf*Nf*D + d*Mf*Nf + y*Mf + x]) >> FIX_POINT_SHIFT);
                sum += B1[d0];
                L1[d0*M0*N0 + y0*M0 + x0] = sum;
            }
}

void conv3()
{
    int M = 12, N = 12, D = 20;
    int Mf = 5, Nf = 5, Df = 50;
    int M0 = M - Mf + 1;
    int N0 = N - Nf + 1;
    for (int d0 = 0; d0 < Df; d0++)
        for (int y0 = 0; y0 < N0; y0++)
            for (int x0 = 0; x0 < M0; x0++) {
                int sum = 0;
                for (int d = 0; d < D; d++)
                    for (int y = 0; y < Nf; y++)
                        for (int x = 0; x < Mf; x++)
                            sum += ((L2[d*M*N + (y + y0)*M + x + x0] * W3[d0*Mf*Nf*D + d*Mf*Nf + y*Mf + x]) >> FIX_POINT_SHIFT);
                sum += B3[d0];
                L3[d0*M0*N0 + y0*M0 + x0] = sum;
            }
}

void conv5()
{
    int M = 4, N = 4, D = 50;
    int Mf = 4, Nf = 4, Df = 500;
    int M0 = M - Mf + 1;
    int N0 = N - Nf + 1;
    for (int d0 = 0; d0 < Df; d0++)
        for (int y0 = 0; y0 < N0; y0++)
            for (int x0 = 0; x0 < M0; x0++) {
                int sum = 0;
                for (int d = 0; d < D; d++)
                    for (int y = 0; y < Nf; y++)
                        for (int x = 0; x < Mf; x++)
                            sum += ((L4[d*M*N + (y + y0)*M + x + x0] * W5[d0*Mf*Nf*D + d*Mf*Nf + y*Mf + x]) >> FIX_POINT_SHIFT);
                sum += B5[d0];
                L5[d0*M0*N0 + y0*M0 + x0] = sum;
            }
}

void conv7()
{
    int M = 1, N = 1, D = 500;
    int Mf = 1, Nf = 1, Df = 10;
    int M0 = M - Mf + 1;
    int N0 = N - Nf + 1;
    for (int d0 = 0; d0 < Df; d0++)
        for (int y0 = 0; y0 < N0; y0++)
            for (int x0 = 0; x0 < M0; x0++) {
                int sum = 0;
                for (int d = 0; d < D; d++)
                    for (int y = 0; y < Nf; y++)
                        for (int x = 0; x < Mf; x++)
                            sum += ((L6[d*M*N + (y + y0)*M + x + x0] * W7[d0*Mf*Nf*D + d*Mf*Nf + y*Mf + x]) >> FIX_POINT_SHIFT);
                sum += B7[d0];
                L7[d0*M0*N0 + y0*M0 + x0] = sum;
            }
}

void pool2()
{
    int M = 24, N = 24, D = 20, stride = 2;
    int M0 = M / stride;
    int N0 = N / stride;
    for (int d0 = 0; d0 < D; d0++)
        for (int y0 = 0; y0 < N; y0 += stride)
            for (int x0 = 0; x0 < M; x0 += stride) {
                int max = L1[d0*M*N + y0*M + x0];
                for (int y = 0; y < stride; y++)
                    for (int x = 0; x < stride; x++) {
                        int v = L1[d0*M*N + (y0 + y)*M + x0 + x];
                        if (v > max)
                            max = v;
                    }
                L2[d0*M0*N0 + (y0/stride)*M0 + x0/stride] = max;
            }
}

void pool4()
{
    int M = 8, N = 8, D = 50, stride = 2;
    int M0 = M / stride;
    int N0 = N / stride;
    for (int d0 = 0; d0 < D; d0++)
        for (int y0 = 0; y0 < N; y0 += stride)
            for (int x0 = 0; x0 < M; x0 += stride) {
                int max = L3[d0*M*N + y0*M + x0];
                for (int y = 0; y < stride; y++)
                    for (int x = 0; x < stride; x++) {
                        int v = L3[d0*M*N + (y0 + y)*M + x0 + x];
                        if (v > max)
                            max = v;
                    }
                L4[d0*M0*N0 + (y0/stride)*M0 + x0/stride] = max;
            }
}

void relu6()
{
    int M = 1, N = 1, D = 500;
    for (int d0 = 0; d0 < D; d0++)
        for (int y0 = 0; y0 < N; y0++)
            for (int x0 = 0; x0 < M; x0++)
                L6[d0*M*N + y0*M + x0] = L5[d0*M*N + y0*M + x0] > 0 ? L5[d0*M*N + y0*M + x0] : 0;
}


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
                            sum += (float)((I[d*M*N + (y + y0)*M + x + x0] * W[d0*Mf*Nf*D + d*Mf*Nf + y*Mf + x]) >> FIX_POINT_SHIFT);
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

int simplenn(const int *image, int *w1, int *b1, int *w3, int *b3, int *w5, int *b5, int *w7, int *b7) {
    int layer1[24*24*20];
    int layer2[12*12*20];
    int layer3[8*8*50];
    int layer4[4*4*50];
    int layer5[1*500];
    int layer6[1*500];
    int layer7[1*10];

    for (int i = 0; i < sizeof(W1)/sizeof(W1[0]); i++)
    	W1[i] = w1[i];
    for (int i = 0; i < sizeof(B1)/sizeof(B1[0]); i++)
    	B1[i] = b1[i];
    for (int i = 0; i < sizeof(W3)/sizeof(W3[0]); i++)
    	W3[i] = w3[i];
    for (int i = 0; i < sizeof(B3)/sizeof(B3[0]); i++)
    	B3[i] = b3[i];
    for (int i = 0; i < sizeof(W5)/sizeof(W5[0]); i++)
    	W5[i] = w5[i];
    for (int i = 0; i < sizeof(B5)/sizeof(B5[0]); i++)
    	B5[i] = b5[i];
    for (int i = 0; i < sizeof(W7)/sizeof(W7[0]); i++)
    	W7[i] = w7[i];
    for (int i = 0; i < sizeof(B7)/sizeof(B7[0]); i++)
    	B7[i] = b7[i];

    for (int i = 0; i < sizeof(I)/sizeof(I[0]); i++)
        I[i] = image[i];


    conv(image,  28, 28,   1, W1, 5, 5,  20, B1, layer1);
    pool(layer1, 24, 24,  20, 2, layer2);
    conv(layer2, 12, 12,  20, W3, 5, 5,  50, B3, layer3);
    pool(layer3,  8,  8,  50, 2, layer4);
    conv(layer4,  4,  4,  50, W5, 4, 4, 500, B5, layer5);
    relu(layer5,  1,  1, 500, layer6);
    conv(layer6,  1,  1, 500, W7, 1, 1,  10, B7, L7);

/*
    conv1();
    pool2();
    conv3();
    pool4();
    conv5();
    relu6();
    conv7();
*/

    int max = L7[0];
    int n = 0;
    for (int i = 1; i < 10; i++)
        if (L7[i] > max) {
            max = L7[i];
            n = i;
    }

    return n;
}

int main()
{
    int bad = 0;
    int w1[5*5*1*20];
    int b1[20];
    int w3[5*5*20*50];
    int b3[50];
    int w5[4*4*50*500];
    int b5[500];
    int w7[1*1*500*10];
    int b7[10];
    int image[28*28];
    clock_t start, end;

//    printf("const int W1[] = {\n");
    for (int i = 0; i < 5*5*1*20; i++) {
        w1[i] = (int)(layer1_weights[i] * FIX_POINT_SCALAR);
//        if (i % 5 == 0) printf("\n");
//        if (i % 25 == 0) printf("\n");
//        printf("%10d, ", w1[i]);
    }
//    printf("};\n\n");
//    printf("const int B1[] = {\n");
    for (int i = 0; i < 20; i++) {
        b1[i] = (int)(layer1_bias[i] * FIX_POINT_SCALAR);
//        if (i % 5 == 0) printf("\n");
//        if (i % 25 == 0) printf("\n");
//        printf("%10d, ", b1[i]);
    }
//    printf("};\n\n");
//    printf("const int W3[] = {\n");
    for (int i = 0; i < 5*5*20*50; i++) {
        w3[i] = (int)(layer3_weights[i] * FIX_POINT_SCALAR);
//        if (i % 5 == 0) printf("\n");
//        if (i % 25 == 0) printf("\n");
//        printf("%10d, ", w3[i]);
    }
//    printf("};\n\n");
//    printf("const int B3[] = {\n");
    for (int i = 0; i < 50; i++) {
        b3[i] = (int)(layer3_bias[i] * FIX_POINT_SCALAR);
//        if (i % 5 == 0) printf("\n");
//        if (i % 25 == 0) printf("\n");
//        printf("%10d, ", b3[i]);
    }
//    printf("};\n\n");
//    printf("const int W5[] = {\n");
    for (int i = 0; i < 4*4*50*500; i++) {
        w5[i] = (int)(layer5_weights[i] * FIX_POINT_SCALAR);
//        if (i % 4 == 0) printf("\n");
//        if (i % 16 == 0) printf("\n");
//        printf("%10d, ", w5[i]);
    }
//    printf("};\n\n");
//    printf("const int B5[] = {\n");
    for (int i = 0; i < 500; i++) {
        b5[i] = (int)(layer5_bias[i] * FIX_POINT_SCALAR);
//        if (i % 10 == 0) printf("\n");
//        if (i % 100 == 0) printf("\n");
//        printf("%10d, ", b5[i]);
    }
//    printf("};\n\n");
//    printf("const int W7[] = {\n");
    for (int i = 0; i < 1*1*500*10; i++) {
        w7[i] = (int)(layer7_weights[i] * FIX_POINT_SCALAR);
//        if (i % 10 == 0) printf("\n");
//        if (i % 100 == 0) printf("\n");
//        printf("%10d, ", w7[i]);
    }
//    printf("};\n\n");
//    printf("const int B7[] = {\n");
    for (int i = 0; i < 10; i++) {
        b7[i] = (int)(layer7_bias[i] * FIX_POINT_SCALAR);
//        printf("%10d, ", b7[i]);
    }
//    printf("};\n\n");

    start = clock();

    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 28*28; j++)
            image[j] = (int)(images[i][j] * FIX_POINT_SCALAR);
        if (simplenn(image, w1, b1,
                            w3, b3,
                            w5, b5,
                            w7, b7) != labels[i] - 1) {
            printf("Bad at %d\n", i + 1);
            bad++;
        }
    }

    end = clock();

    printf("Total bad: %d\n", bad);

    printf("Time taken %lf\n", (double) (end-start) / CLOCKS_PER_SEC);

    return 0;
}
