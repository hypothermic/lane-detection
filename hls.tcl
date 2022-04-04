#!./vinterp.sh
#
# Script for building the HLS Synthesizable Video Processing IP Core
# Also suitable for headless runs like CI/CD
#
# To determine which runs to perform, set the environment variables:
# SIM, SYNTH, COSIM, IMPL
#

# Which hardware to build
#set TARGET_PART	xc7a35ticsg324-1L
set TARGET_PART		xc7z020clg400-1
set TARGET_NAME		lane_vpu
set TARGET_TOP		vpu_accel_top
#set TARGET_LOCATION	"./build/${TARGET_NAME}.prj/${TARGET_NAME}/impl/ip/com_arobs_lane_detection_vpu_accel_0_1.zip"
set TARGET_LOCATION	"./build/${TARGET_NAME}.prj/${TARGET_NAME}/impl/export.zip"
set TARGET_FILE		[file normalize $TARGET_LOCATION]

# Clock period (should be 10 ns because Arty and PYNQ sys clocks both run at 100MHz)
set TARGET_CLOCK_PERIOD	10

# Directories
set ROOT_DIR		.
set ROOT_DIR_REL	${ROOT_DIR}/..
set BUILD_DIR		${ROOT_DIR}/build
set SRC_DIR		${ROOT_DIR_REL}/hls

# Which files to take into account
set SOURCE_FILES	[list vpu_stream.cpp vpu_accel.cpp]
set TEST_FILES		[list vpu_test.cpp]
set TEST_IMAGE		[file normalize ./data/road.ppm]; # generated by data/extract.sh
set TEST_LOGO		[file normalize ./data/ip-core-logo.png]

# Library linkage
set OPENCV_INCLUDE	$::env(OPENCV_INCLUDE)
set OPENCV_LIB		$::env(OPENCV_LIB)
set VISION_INCLUDE	$::env(VISION_INCLUDE)
set CFLAGS		"-I ${OPENCV_INCLUDE} -I ${VISION_INCLUDE} -D __SDSVHLS__ -D __XF__AXI_SDATA__ -std=c++0x"
set LDFLAGS		"-L ${OPENCV_LIB} -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_flann -lopencv_features2d"

# Create the build dir if it doesn't already exist
file mkdir ${BUILD_DIR}

# Vitis only supports path within directory
cd ${BUILD_DIR}
open_project -reset "${TARGET_NAME}.prj"

# Iterate through the source files and add them one by one
foreach f $SOURCE_FILES {
	set pwdd [exec pwd]
	puts "Current dir: $pwdd"
	puts "Adding source file ${SRC_DIR}/${f} to ${TARGET_NAME}"
	add_files ${SRC_DIR}/${f} -cflags ${CFLAGS} -csimflags ${CFLAGS}
}

# Iterate through the test files and add them one by one
foreach f $TEST_FILES {
	puts "Adding unit test file ${SRC_DIR}/${f} to ${TARGET_NAME}"
	add_files -tb ${SRC_DIR}/${f} -cflags ${CFLAGS} -csimflags ${CFLAGS}
}

# Set the top level function
set_top $TARGET_TOP

open_solution -reset $TARGET_NAME -flow_target vivado

set_part $TARGET_PART
create_clock -period $TARGET_CLOCK_PERIOD
set_clock_uncertainty 27.0%

if {[info exists ::env(SIM)]} {
	puts "Running sim"
	csim_design -ldflags ${LDFLAGS} -argv ${TEST_IMAGE}
}

if {[info exists ::env(SYNTH)]} {
	puts "Running synth"
	config_export -format ip_catalog
	config_export -rtl verilog
	config_export -description "Lane Detection Image Processing IP"
	config_export -display_name "VPU Image Processor IP Core"
	config_export -ipname "vpu_accel"
	config_export -library "lane_detection"
	config_export -vendor "com.arobs"
	config_export -version "0.1.0"; # fix for y2k22 bug

	csynth_design
	export_design -format ip_catalog
	exec mkdir -p misc
	exec cp $TEST_LOGO "./misc/logo.png"
	exec zip $TARGET_FILE "misc/logo.png"
	puts "IP Core successfully exported to $TARGET_FILE"
}

puts "Done"
exit 0

