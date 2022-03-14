#!./vinterp.sh
#
# Script for building the hardware outside of the clunky Vivado IDE.
# Also suitable for headless runs like CI/CD
#
# To determine which runs to perform, set the environment variables:
# SYNTH, IMPL, UPLOAD, TIMING
#

# Which hardware to build
set TARGET_PART		xc7a35ticsg324-1L
set TARGET_NAME		lane_hw
set TARGET_TOP		top

# Which runs to execute
set RUN_SYNTH_NAME	synth_1
set RUN_IMPL_NAME	impl_1

# Directories
set ROOT_DIR		.
set BUILD_DIR		${ROOT_DIR}/build
set SRC_DIR		${ROOT_DIR}/rtl

# Hardware definitions, relative paths to SRC_DIR, could use regex/glob...
set CONSTRAINTS_FILE	a7-35.xdc
set SOURCE_FILES	[list top.vhd]

# Create the build dir if it doesn't already exist
file mkdir ${BUILD_DIR}

# Check if all target files exist
foreach f [list {*}$SOURCE_FILES ${CONSTRAINTS_FILE}] {
	puts "Verifying existence of file ${SRC_DIR}/${f}"

	# If a target file doesn't exist, exit with an error message
	if {[file exists ${SRC_DIR}/${f}] == 0} {
		puts "File ${SRC_DIR}/${f} does not exist"
		exit 2
	}
}

# Create or open the project, we don't really care
if {[file exists ${BUILD_DIR}/${TARGET_NAME}.xpr] == 0} {
	puts "Creating project ${TARGET_NAME} for part ${TARGET_PART}"
	create_project ${TARGET_NAME} ${BUILD_DIR} -part ${TARGET_PART}

	# Iterate through the source files and add them one by one
	foreach f $SOURCE_FILES {
		puts "Adding source file ${SRC_DIR}/${f} to ${TARGET_NAME}"
		add_files ${SRC_DIR}/${f}
	}

	# Add the constraints file to it's separate fileset
	puts "Adding constraints file ${CONSTRAINTS_FILE} to ${TARGET_NAME}"
	add_files -fileset constrs_1 ${SRC_DIR}/${CONSTRAINTS_FILE}
} else {
	# Project already exists, open it
	puts "Opening project ${TARGET_NAME} for part ${TARGET_PART}"
	open_project ${BUILD_DIR}/${TARGET_NAME}.xpr
}

# Run a quick syntax check before we proceed, this will save a lot of time
puts "Running preliminary syntax check"
set syntax_errors [check_syntax -return_string]
if {${syntax_errors} != ""} {
	puts "Errors in syntax: $syntax_errors"
	exit 3
}

# Make sure our top module is indeed the top module
puts "Setting file order"
update_compile_order -fileset sources_1
set_property top ${TARGET_TOP} [current_fileset]

if {[info exists ::env(SYNTH)]} {
	# Launch the synthesis run
	puts "Running synthesis, go grab a coffee."
	reset_run ${RUN_SYNTH_NAME}
	launch_runs ${RUN_SYNTH_NAME}
	wait_on_run ${RUN_SYNTH_NAME}

	if {[get_property PROGRESS [get_runs ${RUN_SYNTH_NAME}]] != "100%"} {
		puts "Synth failed with error"
		exit 4
	}
}

open_run ${RUN_SYNTH_NAME} -name netlist_1

if {[info exists ::env(TIMING)]} {
	# Generate timing and power reports after synthesis
	report_timing_summary -delay_type max -report_unconstrained -check_timing_verbose -max_paths 8 -input_pins -file ${BUILD_DIR}/syn_timing.rpt
	report_power -file ${BUILD_DIR}/syn_power.rpt
}

if {[info exists ::env(IMPL)]} {
	# Launch the implementation run and generate the bitstream file
	puts "Running implementation, go eat lunch."
	reset_run ${RUN_IMPL_NAME}
	launch_runs ${RUN_IMPL_NAME} -to_step write_bitstream
	wait_on_run ${RUN_IMPL_NAME}

	# Check the return code of the synth run
	if {[get_property PROGRESS [get_runs ${RUN_IMPL_NAME}]] != "100%"} {
		puts "Implementation failed with error"
		exit 5
	}
}

open_run ${RUN_IMPL_NAME}

if {[info exists ::env(TIMING)]} {
	# Generate timing and power reports after implementation
	report_timing_summary -delay_type min_max -report_unconstrained -check_timing_verbose -max_paths 8 -input_pins -file ${BUILD_DIR}/imp_timing.rpt
	report_power -file ${BUILD_DIR}/imp_power.rpt
}

if {[info exists ::env(UPLOAD)]} {
	# Initialize the hardware manager, the connect command will auto start it
	open_hw_manager
	connect_hw_server -url localhost:3121
	current_hw_target [get_hw_targets */xilinx_tcf/Digilent/*]
	open_hw_target

	# Assume we only have one device: our target device
	set dev [lindex [get_hw_devices] 0]
	current_hw_device $dev
	refresh_hw_device -update_hw_probes false $dev

	# Program the device with the proper bitstream file
	set_property PROGRAM.FILE ${BUILD_DIR}/${TARGET_NAME}.runs/${RUN_IMPL_NAME}/${TARGET_TOP}.bit $dev
	program_hw_devices $dev

	refresh_hw_device $dev
}

exit 0

