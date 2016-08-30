#include "top.hpp"

static int I[28][28];
static int L1[20][24][24];
static int L2[20][12][12];
static int L3[50][8][8];
static int L4[50][4][4];
static int L5[500];
static int L6[500];
static int O[10];


void conv(int L)
{
    int d0, y0, x0, d, x, y;
    int I_copy[5][5];
    int L2_copy[20][5][5];
    int L4_copy[50][4][4];
    int tmp = 0;

    if (L == 1) {
#undef M
#undef N
#undef D
#undef Nf
#undef Mf
#undef Df
#undef M0
#undef N0

#define M    28
#define N    28
#define D     1
#define Nf    5
#define Mf    5
#define Df   20
#define M0   24
#define N0   24

/*
        for (d0 = 0; d0 < Df; d0++)
            for (y0 = 0; y0 < N0; y0++)
#pragma HLS UNROLL factor=2
                for (x0 = 0; x0 < M0; x0++)
#pragma HLS UNROLL factor=2
                    L1[d0*M0*N0 + y0*M0 + x0] = 0;
*/

L1_Loop:
        for (y0 = 0; y0 < N0; y0++) {
            for (x0 = 0; x0 < M0; x0++) {
                for (x = 0; x < Mf; x++)
#pragma HLS PIPELINE
                    for (y = 0; y < Nf; y++)
                        I_copy[y][x] = I[y + y0][x0 + x];

                for (d0 = 0; d0 < Df; d0++) {
#pragma HLS PIPELINE
                    tmp = 0;

                    for (x = 0; x < Mf; x++)
                        for (y = 0; y < Nf; y++)
                            tmp += ((I_copy[y][x] * W1[d0][y][x]) >> FIX_POINT_SHIFT);

                    L1[d0][y0][x0] = tmp;
                }
            }
        }
    } else if (L == 3) {
#undef M
#undef N
#undef D
#undef Nf
#undef Mf
#undef Df
#undef M0
#undef N0

#define M    12
#define N    12
#define D    20
#define Nf    5
#define Mf    5
#define Df   50
#define M0    8
#define N0    8

/*
        for (d0 = 0; d0 < Df; d0++)
            for (y0 = 0; y0 < N0; y0++)
#pragma HLS UNROLL factor=2
                for (x0 = 0; x0 < M0; x0++)
#pragma HLS UNROLL factor=2
                    L3[d0*M0*N0 + y0*M0 + x0] = 0;
*/

L3_Loop:
        for (y0 = 0; y0 < N0; y0++) {
            for (x0 = 0; x0 < M0; x0++) {
                for (x = 0; x < Mf; x++)
#pragma HLS PIPELINE
                    for (y = 0; y < Nf; y++)
                        for (d = 0; d < D; d++)
                            L2_copy[d][y][x] = L2[d][y + y0][x0 + x];

                for (d0 = 0; d0 < Df; d0++) {
                    tmp = 0;

                    for (x = 0; x < Mf; x++)
#pragma HLS PIPELINE
                        for (y = 0; y < Nf; y++)
                            for (d = 0; d < D; d++)
                            	tmp += ((L2_copy[d][y][x] * W3[d0][d][y][x]) >> FIX_POINT_SHIFT);

                    L3[d0][y0][x0] = tmp;
                }
            }
        }
    } else if (L == 5) {
#undef M
#undef N
#undef D
#undef Nf
#undef Mf
#undef Df
#undef M0
#undef N0

#define M     4
#define N     4
#define D    50
#define Nf    4
#define Mf    4
#define Df  500
#define M0    1
#define N0    1

/*
        for (d0 = 0; d0 < Df; d0++)
//#pragma HLS UNROLL factor=2
            L5[d0*M0*N0] = 0;
*/

L5_Loop:
        for (d0 = 0; d0 < Df; d0++) {
            tmp = 0;

            for (x = 0; x < Mf; x++)
#pragma HLS PIPELINE
                for (y = 0; y < Nf; y++)
                    for (d = 0; d < D; d++)
                        tmp += ((L4[d][y][x] * W5[d0][d][y][x]) >> FIX_POINT_SHIFT);

            L5[d0] = tmp;
        }
    } else { /* L == 7 */
#undef M
#undef N
#undef D
#undef Nf
#undef Mf
#undef Df
#undef M0
#undef N0

#define M     1
#define N     1
#define D   500
#define Nf    1
#define Mf    1
#define Df   10
#define M0    1
#define N0    1

/*
        for (d0 = 0; d0 < Df; d0++)
//#pragma HLS UNROLL factor=2
            O[d0*M0*N0] = B7[d0];
*/

L7_Loop:
        for (d0 = 0; d0 < Df; d0++) {
#pragma HLS PIPELINE
            tmp = 0;

            for (d = 0; d < D; d++)
                tmp += ((L6[d] * W7[d0][d]) >> FIX_POINT_SHIFT);

            O[d0] = tmp;
        }
    }
}

void pool(int L)
{
    if (L == 2) {
#undef M
#undef N
#undef D
#undef STRIDE
#undef M0
#undef N0

#define M    24
#define N    24
#define D    20
#define STRIDE 2
#define M0   (M / STRIDE)
#define N0   (N / STRIDE)

        for (int d0 = 0; d0 < D; d0++)
            for (int y0 = 0, y1 = 0; y0 < N; y0 += STRIDE, y1++)
#pragma HLS PIPELINE
                for (int x0 = 0, x1 = 0; x0 < M; x0 += STRIDE, x1++)
                    L2[d0][y1][x1] = L1[d0][y0][x0];

L2_Loop:
        for (int d0 = 0; d0 < D; d0++)
            for (int y0 = 0, y1 = 0; y0 < N; y0 += STRIDE, y1++)
                for (int x0 = 0, x1 = 0; x0 < M; x0 += STRIDE, x1++)
                    for (int y = 0; y < STRIDE; y++)
#pragma HLS PIPELINE
                        for (int x = 0; x < STRIDE; x++)
                            L2[d0][y1][x1] =
                                (L1[d0][y0 + y][x0 + x] > L2[d0][y1][x1]) ?
                                    L1[d0][y0 + y][x0 + x] : L2[d0][y1][x1];
    } else { /* L == 4 */
#undef M
#undef N
#undef D
#undef STRIDE
#undef M0
#undef N0

#define M     8
#define N     8
#define D    50
#define STRIDE 2
#define M0   (M / STRIDE)
#define N0   (N / STRIDE)

        for (int d0 = 0; d0 < D; d0++)
            for (int y0 = 0, y1 = 0; y0 < N; y0 += STRIDE, y1++)
#pragma HLS PIPELINE
                for (int x0 = 0, x1 = 0; x0 < M; x0 += STRIDE, x1++)
                    L4[d0][y1][x1] = L3[d0][y0][x0];

L4_Loop:
        for (int d0 = 0; d0 < D; d0++)
            for (int y0 = 0, y1 = 0; y0 < N; y0 += STRIDE, y1++)
                for (int x0 = 0, x1 = 0; x0 < M; x0 += STRIDE, x1++)
#pragma HLS PIPELINE
                    for (int y = 0; y < STRIDE; y++)
                        for (int x = 0; x < STRIDE; x++)
                            L4[d0][y1][x1] =
                                (L3[d0][y0 + y][x0 + x] > L4[d0][y1][x1]) ?
                                    L3[d0][y0 + y][x0 + x] : L4[d0][y1][x1];
    }
}

void relu()
{
    /*
    int M = 1, N = 1, D = 500;
    for (int d0 = 0; d0 < D; d0++)
        for (int y0 = 0; y0 < N; y0++)
            for (int x0 = 0; x0 < M; x0++)
                I[d0*M*N + y0*M + x0] = O[d0*M*N + y0*M + x0] > 0 ? O[d0*M*N + y0*M + x0] : 0;
    */
#define RELU_D 500
    for (int d0 = 0; d0 < RELU_D; d0++)
#pragma HLS PIPELINE
        L6[d0] = L5[d0] > 0 ? L5[d0] : 0;
}

void top(const int image[28*28], int *ret)
{
    int i, j;

    for (i = 0; i < 28; i++)
        for (j = 0; j < 28; j++)
        I[i][j] = image[i*28 + j];

    conv(1);
    pool(2);
    conv(3);
    pool(4);
    conv(5);
    relu();
    conv(7);

    int max = O[0];
    int n = 0;
    for (int i = 1; i < 10; i++)
        if (O[i] > max) {
            max = O[i];
            n = i;
    }

    *ret = n;
}
