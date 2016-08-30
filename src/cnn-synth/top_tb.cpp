#include "top.hpp"
#include <iostream>
#include <time.h>

#include "images_small.h"

int main() {
    int ret, bad = 0;
    int image[28*28];
    clock_t start, end;

    start = clock();

    ap_int<896> temp_input = 0;

    for (int i = 0; i < 5; i++) {
        temp_input = 0; //reset temp_input to 0

        for (int j = 0; j < 28*28; j++) {
            image[j] = (int)(images[i][j] * FIX_POINT_SCALAR);
            /*
            ap_int<896> temp = (int)(images[i][j] * FIX_POINT_SCALAR);
            ap_int<896> mask = 0xFFFFFFFF;

            temp_input = temp_input + ((temp << (j*32)) & (mask << (j*32)));
            ap_int<896> d_out = ((temp << (j*32)) & (mask << (j*32)));

            if (i == 1 && j <= 28) {
                std::cout << "d_out: " << temp << std::endl;
                std::cout << "temp_input: " << temp_input << std::endl;
            }
            */
        }
        top(image, &ret);
        if (ret != labels[i] - 1) {
            std::cout << "Bad at " << i + 1 << "\n";
            bad++;
        }
    }

    end = clock();

    std::cout << "Total bad: " << bad << "\n";

    std::cout << "Time taken: " << (double) (end-start) << "\n";

	return 0;
}
