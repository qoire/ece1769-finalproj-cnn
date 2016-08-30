#include "Vdot_product.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char **argv, char **env){

	int i;
	if(argc<2)
	{
		printf("invalid argurment\n");
		exit(1);
	}
	char *file_name = argv[1];
	Verilated::commandArgs(argc,argv);
	Vdot_product* top = new Vdot_product;
	Verilated::traceEverOn(true);

	VerilatedVcdC* tfp = new VerilatedVcdC;
  	top->trace (tfp, 99);
	tfp->open(file_name);

	top->ap_rst = 1;
	top->ap_clk = 0;
	i=0;
	while(true) {
		top->ap_start = (i==2 || i==3);
		top->ap_rst = (i<2);
		tfp->dump(i);
		top->ap_clk = !top->ap_clk;
		top->eval();
		if(top->ap_done){
			for(int j=0;j<4;j++){
				i++;
				tfp->dump(i);
				top->ap_clk = !top->ap_clk;
				top->eval();
			}
			tfp->dump(i);
			break;
		}
		i++;
	}
	tfp->close();
	delete top;
	exit(0);
}
