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

#include "vpu_accel.hpp"

/**
 * Width for the test output image
 */
#define TEST_OUT_WIDTH	(VPU_IMAGE_WIDTH  / 2)

/**
 * Height of the test output image
 */
#define TEST_OUT_HEIGHT	(VPU_IMAGE_HEIGHT / 2)

int main(int argc, char **argv) {
	cv::Mat src, sw, hw, diff;
	float error;

	if (argc != 2) {
		std::cerr << "Provide an image path as argument!" << std::endl;
		return 1;
	}

	src = cv::imread(argv[1], 0);

	if (src.data == NULL) {
		std::cerr << "Failed to read image: " << argv[1] << std::endl;
		return 2;
	}

	sw.create(TEST_OUT_WIDTH, TEST_OUT_HEIGHT, CV_8UC3);
	hw.create(TEST_OUT_WIDTH, TEST_OUT_HEIGHT, CV_8UC3);
	diff.create(TEST_OUT_WIDTH, TEST_OUT_HEIGHT, CV_8UC3);

	cv::resize(src, sw, cv::Size(TEST_OUT_WIDTH, TEST_OUT_HEIGHT), 0, 0, CV_INTER_LINEAR);
	
	vpu_accel_top((ap_uint<VPU_IMAGE_WIDTH> *) src.data, (ap_uint<VPU_IMAGE_WIDTH> *) hw.data, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH);

	absdiff(sw, hw, diff);

	cv::imwrite("test_sw.png", sw);
	cv::imwrite("test_hw.png", hw);
	cv::imwrite("test_diff.png", diff);

	xf::cv::analyzeDiff(diff, false, error);

	// Allow less than 5 percent difference
	if (error > 0.05f) {
		std::cerr << "VPU Test failed with " << error << "% difference" << std::endl;
		return 3;
	} else {
		std::cout << "VPU Test OK" << std::endl;
		return 0;
	}
}

