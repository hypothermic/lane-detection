/**
 * @file vpu_test.cpp
 * @author Matthijs Bakker
 * @brief Test the simulated accelerator function against a system OpenCV result
 *
 * Here we run the HLS-synthesis simulation against a OpenCV running on the host PC to check
 * if the output images are identical and thus if the synthesized RTL is correct.
 */
#include <stdio.h>
#include <stdlib.h>

#include <common/xf_headers.hpp>
#include <common/xf_axi.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "vpu_accel.hpp"

/**
 * Width for the test output image
 */
#define TEST_IMAGE_WIDTH	(/*VPU_IMAGE_WIDTH*/1280)

/**
 * Height of the test output image
 */
#define TEST_IMAGE_HEIGHT	(/*VPU_IMAGE_HEIGHT*/720)

/**
 * Maximum percentage of difference between the OCV-
 * generated image and the VPU generated image
 */
#define TEST_MAX_DIFF		(0.05f)

/**
 * @block
 *
 * Exit code for when certain tests fail
 */
#define EXIT_FAILURE_READ	(EXIT_FAILURE + 0)
#define EXIT_FAILURE_EMPTY	(EXIT_FAILURE + 1)
#define EXIT_FAILURE_DIFF	(EXIT_FAILURE + 2)

/**
 * Apply the filters in software using OpenCV to create a reference image
 */
static void apply_ocv(cv::Mat &src, cv::Mat &dst) {
	cv::Mat i1;

	i1.create(src.rows, src.cols, CV_8UC1);

	cv::cvtColor(src, i1, cv::COLOR_BGR2GRAY);
	cv::Sobel(i1, dst, CV_8UC1, 1, 1, 3, 1, 0, cv::BORDER_CONSTANT);
}

/**
 * Run the image through the hardware kernel
 *
 * Depending if csim or cosim is used, Vitis workflow will adapt and
 * automatically choose if the executable runs in XSim or not
 */
static void apply_hw(cv::Mat &src, cv::Mat &dst) {
	hls_stream_t<VPU_IMAGE_INPUT_TYPE> hw_in_stream;
	hls_stream_t<VPU_IMAGE_OUTPUT_TYPE> hw_out_stream;
	
	xf::cv::cvMat2AXIvideoxf<XF_NPPC1, VPU_IMAGE_INPUT_THT>(src, hw_in_stream);
	vpu_accel_top(hw_in_stream, hw_out_stream, src.rows, src.cols);
	xf::cv::AXIvideo2cvMatxf<XF_NPPC1>(hw_out_stream, dst);
}

/**
 * Entry point function which loads the image from disk and runs tests
 */
int main(int argc, char **argv) {
	cv::Mat src, sw, hw, diff;
	float error;

	// Check if user supplied a path argument when running the executable
	if (argc != 2) {
		std::cerr << "Provide an image path as argument!" << std::endl;
		return EXIT_FAILURE_READ;
	}

	// Load the image from disk
	src = cv::imread(argv[1], cv::IMREAD_COLOR);

	// Check if the image is loaded correctly by measuring if it has data
	if (src.empty()) {
		std::cerr << "Failed to read image: " << argv[1] << std::endl;
		return EXIT_FAILURE_EMPTY;
	}

	std::cout << "Image from: " << argv[1]  << " with res " << src.cols << "x" << src.rows << " px" << std::endl;

	// Allocate memory to store the results
	sw.create(src.rows, src.cols, CV_8UC1);
	hw.create(src.rows, src.cols, CV_8UC1);
	diff.create(src.rows, src.cols, CV_8UC1);

	apply_ocv(src, sw);
	apply_hw(src, hw);

	// Calculate how much the images differ
	absdiff(sw, hw, diff);
	xf::cv::analyzeDiff(diff, false, error);

	// Write results to file for visual inspection
	cv::imwrite("test_sw.png", sw);
	cv::imwrite("test_hw.png", hw);
	cv::imwrite("test_diff.png", diff);

	// Allow less than X percent difference
	if (error > TEST_MAX_DIFF) {
		std::cerr << "VPU Test failed with " << error << "% difference" << std::endl;
		return EXIT_FAILURE_DIFF;
	} else {
		std::cout << "VPU Test OK" << std::endl;
		return EXIT_SUCCESS;
	}
}

