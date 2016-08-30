include $(LEVEL)/site.mk

###################################
# Project Name & Solution Name
###################################
PROJ_NAME=cnn_synth
SOLUTION_NAME=sol
###################################
# Find all Verilog files
###################################
RTL_FILES	= $(shell find . -maxdepth 1 -name "*.v")

TEST_BENCH=$(LEVEL)/tb/cnn_synth_tb_verilator.cpp

all :	verilate

###################################
# HLS PROJECT AND SOLUTION NAMES
###################################
HLS		=	$(VIVADO_HLS_PATH)/vivado_hls

$(TOP_MODULE).v :
	$(HLS) script.tcl $(PROJ_NAME) $(SOLUTION_NAME)
	cp $(PROJ_NAME)/$(SOLUTION_NAME)/syn/verilog/* .

csyn: $(TOP_MODULE).v

###################################
# Verilator rules
###################################
VIEW_OPTIONS	= --save=wave.gtkw
VV				= $(VERILATOR_PATH)/verilator
VFLAGS			= --trace -Wno-WIDTH
CFLAGS			= -j
YY				= $(YOSYS_PATH)/yosys

verilate: $(TOP_MODULE).verilated

$(TOP_MODULE).verilated: $(TOP_MODULE).v $(TEST_BENCH)
	echo $(VERILATOR_PATH)
	$(VV) $(VFLAGS)  --cc $(TOP_MODULE).v --exe $(TEST_BENCH)
	cd obj_dir && \
	make $(CFLAGS) -f V$(TOP_MODULE).mk V$(TOP_MODULE) && \
	mv V$(TOP_MODULE) ../$(TOP_MODULE).verilated

###################################
# Simulation rules
###################################

sim: $(TOP_MODULE).vcd

$(TOP_MODULE).vcd: $(TOP_MODULE).verilated
	./$(TOP_MODULE).verilated $(TOP_MODULE).vcd

###################################
# Waveform viewing
###################################

view: $(TOP_MODULE).vcd
	$(GTKWAVE_PATH)/gtkwave $(VIEW_OPTIONS) $(TOP_MODULE).vcd

###################################
# Yosys rules
###################################
YOSYS_GLOBRST		?=
YOSYS_COARSE		?=
YOSYS_SPLITNETS		?=

synth.ys: $(TOP_MODULE).v
	for file in $(RTL_FILES); do                     		\
		echo "read_verilog $$file" >> $@ 			;\
	done
	if test -n "$(TOP_MODULE)"; then                          	\
		echo "hierarchy -check -top $(TOP_MODULE)" >> $@ 	;\
	else                                                		\
		echo "hierarchy -check" >> $@ 				;\
	fi
	if test -z "$(YOSYS_GLOBRST)"; then                 		\
		echo "add -global_input globrst 1" >> $@ 		;\
		echo "proc -global_arst globrst" >> $@   		;\
	else                                                		\
		echo "proc" >> $@                        		;\
	fi
	echo "flatten; opt; memory; opt; fsm; opt" >> $@
	if test -n "$(YOSYS_COARSE)"; then                    		\
		echo "techmap; opt; abc -dff; clean" >> $@ 		;\
	fi
	if test -z "$(YOSYS_SPLITNETS)"; then				\
		echo "splitnets; clean"  >> $@             		;\
	fi
	if test -z "$(YOSYS_COARSE)"; then 				\
		echo "write_verilog -noexpr -noattr synth.v"  >> $@ 	;\
	else 								\
		echo "select -assert-none t:\$[!_]">> $@    		;\
		echo "write_verilog -noattr synth.v" >> $@ 		;\
	fi
	echo "stat" >> $@

synth.v: synth.ys $(RTL_FILES)
	$(YY) -v2 -l synth.log synth.ys

synth.stat : synth.v
	$(YY) -l synth.stat -p "read_verilog synth.v" -p stat

syn:    synth.v
stat:   synth.stat
###################################
# clearning rules
###################################
.PHONY: clean

TCL_CLEAN = $(wildcard ./*/)

clean:
	@rm -rf ./obj_dir ./$(PROJ_NAME) V$(TOP_MODULE)
	@rm -f *.vcd *.log *.verilated $(CSYN_FILES)
	@rm -f *.ys synth.v *.stat
	@rm -rf ./cnn-c/cnn/syn
	find . -maxdepth 1 -type f -name "*tcl" \
	-not -name 'script.tcl' \
	-not -name 'directives.tcl' \
	-not -name '.*' -print0 | xargs -0 rm -f --
