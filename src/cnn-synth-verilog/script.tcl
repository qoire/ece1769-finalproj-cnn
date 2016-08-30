############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2016 Xilinx, Inc. All Rights Reserved.
############################################################
if { $argc != 2 } {
	puts "The script.tcl requires two input arguments: Project name, and solution name".
	puts "Please provide them!"
} else {
open_project [lindex $argv 0]
set_top top
add_files top.cpp
add_files top.hpp
add_files -tb top_tb.cpp
open_solution [lindex $argv 1]
set_part {xc7a50tcsg324-2l}
create_clock -period 10 -name default
csim_design
csynth_design
#cosim_design
}
