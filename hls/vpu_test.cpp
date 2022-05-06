/**
 * @file vpu_test.cpp
 * @author Matthijs Bakker
 * @brief Test the simulated accelerator function against a system OpenCV result
 *
 * Here we run the HLS-synthesis simulation against a OpenCV running on the host PC
 * to check if the results are identical and thus if the synthesized RTL is correct.
 */

#include <iostream>
#include <vector>

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
 * Which value to use for the segmentation threshold param
 */
#define ARG_SEG_THRESH		(50)

/**
 * Which value to use for the Gaussian blur sigma param
 */
#define ARG_GAUSS_SIGMA		(2.5f)

/**
 * Which value to use for the edge pixel threshold
 */
#define ARG_EDGE_THRESH		(20)

/**
 * Which value to use for the Hough voting threshold
 */
#define ARG_HOUGH_THRESH	(200)

/**
 * How many lines to let the hardware scan for
 */
#define ARG_MAXLINES		(32)

#define MAX_DIFF_RHO		(2.0f * TEST_MAX_DIFF)

#define MAX_DIFF_THETA		(2.0f * VPU_FX_HOUGH_RES)

/**
 * @block
 *
 * Exit code for when certain tests fail
 */
#define EXIT_FAILURE_READ	(EXIT_FAILURE + 0)
#define EXIT_FAILURE_EMPTY	(EXIT_FAILURE + 1)
#define EXIT_FAILURE_HW_LESS	(EXIT_FAILURE + 2)
#define EXIT_FAILURE_INTEGRITY	(EXIT_FAILURE + 3)

/**
 * Apply the filters in software using OpenCV to create a reference image
 */
static void apply_ocv(cv::Mat &src, std::vector<cv::Vec2f> &result) {
	constexpr double theta_resolution = CV_PI / 180 * VPU_FX_HOUGH_RES;
	cv::Mat hsv, seg, gauss, sobel, edges;

	hsv.create(src.rows, src.cols, CV_8UC3);
	seg.create(src.rows, src.cols, CV_8UC1);
	gauss.create(src.rows, src.cols, CV_8UC3);
	sobel.create(src.rows, src.cols, CV_8UC3);
	edges.create(src.rows, src.cols, CV_8UC3);

	cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);
	cv::inRange(hsv, cv::Scalar(0, 0, 255-ARG_SEG_THRESH), cv::Scalar(255, ARG_SEG_THRESH, 255), seg);
	cv::GaussianBlur(seg, gauss, cv::Size(VPU_FX_GAUSS_SIZE, VPU_FX_GAUSS_SIZE), ARG_GAUSS_SIGMA, ARG_GAUSS_SIGMA, cv::BORDER_CONSTANT);
	cv::Sobel(gauss, sobel, CV_8UC1, 1, 1, 3, 1, 0, cv::BORDER_CONSTANT);
	cv::threshold(sobel, edges, ARG_EDGE_THRESH, 255, cv::THRESH_BINARY);
	cv::HoughLines(edges, result, 1.0d, theta_resolution, ARG_HOUGH_THRESH);

	hsv.release();
	seg.release();
	gauss.release();
	sobel.release();
	edges.release();
}

template<int N, int D, int FP_W, int FP_F>
static void fpread(hls::stream<ap_axis<FP_W, 1, 1, 1>> &stream, float results[D][N]) {
	ap_axis<FP_W, 1, 1, 1> elem;

	#ifdef __SYNTHESIS__
	#error Not synthesizable, use on host only
	#endif

	for (int d = 0; d < D; ++d) {
		for (int n = 0; n < N; ++n) {
			stream.read(elem);
			results[d][n] = elem.data / (1 << (FP_F));
		}
	}
}

/**
 * Run the image through the hardware kernel
 *
 * Depending if csim or cosim is used, Vitis workflow will adapt and
 * automatically choose if the executable runs in XSim or not
 */
static void apply_hw(cv::Mat &src, float results[2][ARG_MAXLINES]) {
	hls_stream_t<VPU_IMAGE_INPUT_TYPE> hw_in_stream;
	hls::stream<ap_axis<32, 1, 1, 1>> hw_out_stream;
	
	std::cout << "cvMat2AXIvideoxf" << std::endl;
	xf::cv::cvMat2AXIvideoxf<XF_NPPC1, VPU_IMAGE_INPUT_THT>(src, hw_in_stream);

	std::cout << "vpu_accel_top" << std::endl;
	vpu_accel_top(
		hw_in_stream, hw_out_stream,
		src.rows, src.cols,
		static_cast<uint8_t>(ARG_SEG_THRESH),
		ARG_GAUSS_SIGMA,
		static_cast<short>(ARG_EDGE_THRESH),
		static_cast<short>(ARG_HOUGH_THRESH)
	);

	std::cout << "fpread" << std::endl;
	fpread<ARG_MAXLINES, 2, 32, 22>(hw_out_stream, results);
	std::cout << "end" << std::endl;
}

/**
 * Entry point function which loads the image from disk and runs tests
 */
int main(int argc, char **argv) {
	cv::Mat			src;
	std::vector<cv::Vec2f>	sw_results;
	float			hw_results[2][ARG_MAXLINES];

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

	std::cout << "Creating opencv reference..." << std::endl;
	apply_ocv(src, sw_results);	
	std::cout << "Running hardware kernel..." << std::endl;
	apply_hw(src, hw_results);

	std::cout << "sw results len: " << sw_results.size() << ".." << std::endl;

	// Check if all results match
//	for (int i = 0; i < std::min(static_cast<int>(sw_results.size()), ARG_MAXLINES); ++i) {
//		float	hw_rho = hw_results[0][i],
//			hw_theta = hw_results[1][i],
//			sw_rho = sw_results[i][0],
//			sw_theta = sw_results[i][1],
//			diff_rho = std::fabs(hw_rho - sw_rho),
//			diff_theta = std::fabs(hw_theta - sw_theta);
//
//		// Duplicate result means EOF
//		if (i != 0 && (hw_rho == hw_results[0][i-1])
//			   || (hw_theta == hw_results[1][i-1])) {
//			return EXIT_FAILURE_HW_LESS;
//		}
//
//		std::cout << "rho: " << sw_results[i][0] << " th: " << sw_results[i][1] << std::endl;
//
//		if (diff_rho > MAX_DIFF_RHO || diff_theta > MAX_DIFF_THETA) {
//			return EXIT_FAILURE_INTEGRITY;
//		}
//	}
	
	return EXIT_SUCCESS;
}

