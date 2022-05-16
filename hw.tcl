#!./vinterp.sh
#
# Script for building the hardware outside of the clunky Vivado IDE.
# Also suitable for headless runs like CI/CD
#
# To determine which runs to perform, set the environment variables:
# SYNTH, IMPL, UPLOAD, TIMING
#

# Which hardware to build
#set TARGET_PART	xc7a35ticsg324-1L
set TARGET_PART		xc7z020clg400-1
set TARGET_NAME		lane_hw
#set TARGET_TOP		top
set TARGET_TOP		bd_wrapper

# Which runs to execute
set RUN_SYNTH_NAME	synth_1
set RUN_IMPL_NAME	impl_1

# Directories
set ROOT_DIR		.
set BUILD_DIR		${ROOT_DIR}/build
set SRC_DIR		${ROOT_DIR}/rtl
set IP_DIR		[file normalize ${BUILD_DIR}/lane_vpu.prj/lane_vpu/impl/ip]

# Hardware definitions, relative paths to SRC_DIR, could use regex/glob...
set CONSTRAINTS_FILE	constraints.xdc
set SOURCE_FILES	[list top.vhd]
set IPI_BD_FILE		ibd.tcl
set IPI_SRC_FILE	[file normalize ${IP_DIR}/com_arobs_lane_detection_vpu_accel_0_1.zip]

# Create the build dir if it doesn't already exist
file mkdir ${BUILD_DIR}

# Check if all target files exist
foreach f [list {*}$SOURCE_FILES ${CONSTRAINTS_FILE}] {
	common::send_gid_msg -ssname BD::TCL -id 2001 -severity "INFO" "Verifying existence of file ${SRC_DIR}/${f}"

	# If a target file doesn't exist, exit with an error message
	if {[file exists ${SRC_DIR}/${f}] == 0} {
		common::send_gid_msg -ssname BD::TCL -id 2005 -severity "ERROR" "File ${SRC_DIR}/${f} does not exist"
		exit 2
	}
}

# Create or open the project, we don't really care
if {[file exists ${BUILD_DIR}/${TARGET_NAME}.xpr] == 0} {
	common::send_gid_msg -ssname BD::TCL -id 2004 -severity "INFO" "Creating project on disk for ${TARGET_NAME} for part ${TARGET_PART}"
	create_project ${TARGET_NAME} ${BUILD_DIR} -part ${TARGET_PART}

	# Iterate through the source files and add them one by one
	foreach f $SOURCE_FILES {
		common::send_gid_msg -ssname BD::TCL -id 2003 -severity "INFO" "Adding source file ${SRC_DIR}/${f} to ${TARGET_NAME}"
		#add_files ${SRC_DIR}/${f}
	}

	# Add the constraints file to it's separate fileset
	common::send_gid_msg -ssname BD::TCL -id 2002 -severity "INFO" "Adding constraints file ${CONSTRAINTS_FILE} to ${TARGET_NAME}"
	add_files -fileset constrs_1 ${SRC_DIR}/${CONSTRAINTS_FILE}

	# Import the VPU IP core into the IP repositories
	# For some odd reason we have to update the IP catalog twice, else vivado won't catch it
	set_property ip_repo_paths "${IP_DIR}" [current_project]
	update_ip_catalog
	update_ip_catalog -add_ip "${IPI_SRC_FILE}" -repo_path "${IP_DIR}"

	# Run block design instantiation script
	common::send_gid_msg -ssname BD::TCL -id 2001 -severity "INFO" "Instantiating block design from file ${SRC_DIR}/${IPI_BD_FILE}"
	source "${SRC_DIR}/${IPI_BD_FILE}"
	make_wrapper -top -files [get_files ${BUILD_DIR}/${TARGET_NAME}.srcs/sources_1/bd/bd/bd.bd]
	add_files -norecurse ${BUILD_DIR}/${TARGET_NAME}.srcs/sources_1/bd/bd/hdl/bd_wrapper.v
} else {
	# Project already exists, open it
	common::send_gid_msg -ssname BD::TCL -id 2000 -severity "INFO" "Opening project ${TARGET_NAME} for part ${TARGET_PART}"
	open_project ${BUILD_DIR}/${TARGET_NAME}.xpr
}

# Run a quick syntax check before we proceed, this will save a lot of time
common::send_gid_msg -ssname BD::TCL -id 2009 -severity "INFO" "Running preliminary syntax check"
set syntax_errors [check_syntax -return_string]
if {${syntax_errors} != ""} {
	common::send_gid_msg -ssname BD::TCL -id 2008 -severity "ERROR" "Syntax error(s): $syntax_errors"
	exit 3
}

# Make sure our top module is indeed the top module
set_property top ${TARGET_TOP} [current_fileset]
update_compile_order -fileset sources_1

if {[info exists ::env(SYNTH)]} {
	# Launch the synthesis run
	common::send_gid_msg -ssname BD::TCL -id 2006 -severity "INFO" "Running synthesis, go grab a coffee"
	reset_run ${RUN_SYNTH_NAME}
	launch_runs ${RUN_SYNTH_NAME}
	wait_on_run ${RUN_SYNTH_NAME}

	if {[get_property PROGRESS [get_runs ${RUN_SYNTH_NAME}]] != "100%"} {
		common::send_gid_msg -ssname BD::TCL -id 2007 -severity "ERROR" "Synth failed with error"
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
	common::send_gid_msg -ssname BD::TCL -id 2004 -severity "INFO" "Running implementation, go eat lunch."
	reset_run ${RUN_IMPL_NAME}
	launch_runs ${RUN_IMPL_NAME} -to_step write_bitstream
	wait_on_run ${RUN_IMPL_NAME}

	# Check the return code of the synth run
	if {[get_property PROGRESS [get_runs ${RUN_IMPL_NAME}]] != "100%"} {
		common::send_gid_msg -ssname BD::TCL -id 2005 -severity "ERROR" "Implementation failed with error"
		exit 5
	}

	common::send_gid_msg -ssname BD::TCL -id 2002 -severity "INFO" "Generated bitstream: ./build/lane_hw.runs/impl_1/bd_wrapper.bit"
	common::send_gid_msg -ssname BD::TCL -id 2003 -severity "INFO" "Generated hwhandoff: ./build/lane_hw.gen/sources_1/bd/bd/hw_handoff/bd.hwh"
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

	common::send_gid_msg -ssname BD::TCL -id 2001 -severity "INFO" "Programming done"
}

exit 0

