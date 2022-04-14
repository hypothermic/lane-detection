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

int main(int argc, char **argv) {
	cv::Mat src, sw1, sw2, hw, diff;
	hls::stream<ap_axiu<24, 1, 1, 1>> hw_in_stream;
	hls::stream<ap_axiu<8, 1, 1, 1>> hw_out_stream;
	float error;

	if (argc != 2) {
		std::cerr << "Provide an image path as argument!" << std::endl;
		return 1;
	}

	src = cv::imread(argv[1], cv::IMREAD_COLOR);

	if (src.data == NULL) {
		std::cerr << "Failed to read image: " << argv[1] << std::endl;
		return 2;
	}

	std::cout << "Image from: " << argv[1]  << " with res " << src.rows << "x" << src.cols << " px" << std::endl;

	sw1.create(src.rows, src.cols, CV_8UC1);
	sw2.create(src.rows, src.cols, CV_8UC1);
	hw.create(src.rows, src.cols, CV_8UC1);
	diff.create(src.rows, src.cols, CV_8UC1);

	cv::cvtColor(src, sw1, cv::COLOR_BGR2GRAY);

	cv::Sobel(sw1, sw2, CV_8UC1, 1, 1, 3, 1, 0, cv::BORDER_CONSTANT);
	
	xf::cv::cvMat2AXIvideoxf<XF_NPPC1, 24>(src, hw_in_stream);
	vpu_accel_top(hw_in_stream, hw_out_stream, src.rows, src.cols);
	xf::cv::AXIvideo2cvMatxf<XF_NPPC1>(hw_out_stream, hw);

	absdiff(sw2, hw, diff);

	cv::imwrite("test_sw.png", sw2);
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

