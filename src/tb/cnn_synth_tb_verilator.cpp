#include "Vtop.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include "images.h"

#define FIX_POINT_SCALAR 256
#define NUM_TESTS 10000

// *****************************
// TEST BENCH MAIN
// *****************************
int main(int argc, char **argv, char **env) {

	if(argc<2)
	{
		printf("invalid argurment\n");
		exit(1);
	}
	Verilated::commandArgs(argc, argv);
	Vtop *top = new Vtop;

	char *file_name = argv[1];

	VL_PRINTF("Enabling waves...\n");
	Verilated::traceEverOn(true);
	VerilatedVcdC* tfp = new VerilatedVcdC;
	top->trace(tfp, 99);
	tfp->open(file_name);

	int temp = 5;

	// setup for testing
	int i, k, i0, bad;
	int prev_ce0 = 0;
	top->ap_rst = 1;
	top->ap_clk = 0;
	i = 0;
	k = 0;
	i0 = 0;
	bad = 0;
	while(true) {
		top->ap_start = (i == 2 || i == 3);
		top->ap_rst = (i < 2);
		if (i0 == 0)
			tfp->dump(i);

		top->ap_clk = !top->ap_clk;
		top->eval();

		if (top->image_r_ce0 == 1 && prev_ce0 == 0) {
			prev_ce0 = 1;
			if (top->image_r_address0 > 0)
				top->image_r_q0 = (int)(images[i0][top->image_r_address0 - 1] * FIX_POINT_SCALAR);
		}
		prev_ce0 = top->image_r_ce0;

		if (top->ap_done) {
			if (i0 < NUM_TESTS) {
				if (top->ret != labels[i0] - 1) {
					bad++;
					VL_PRINTF("Bad at %d\n", i0);
				} 

				i0++;
				prev_ce0 = 0;
				top->ap_rst = 1;

				for (int j = 0; j < 4; j++) {
					i++;

					top->ap_start = (j == 2 || j == 3);
			                top->ap_rst = (j < 2);

					if (i0 == 1)
						tfp->dump(i);
					top->ap_clk = !top->ap_clk;
					top->eval();

				}
			} else {
				for (int j = 0; j < 100; j++) {
					i++;
					tfp->dump(i);
					top->ap_clk = !top->ap_clk;
					top->eval();
				}
				tfp->dump(i);
				break;
			}
		}
		i++;
	}

        VL_PRINTF("Total bad: %d\n", bad);

	VL_PRINTF("Total iterations: %i", i);

	// close output file
	tfp->close();
	// delete top when done
	delete top;
	exit(0);
}
