/**
 * @file vpu_test.cpp
 * @author Matthijs Bakker
 * @brief Test the simulated accelerator function against a system OpenCV result
 *
 * Here we run the HLS-simulation and/or cosimulation against OpenCV running on the host
 * to check if the results are identical and thus if the created RTL is correct.
 */

#include <iostream>
#include <vector>

#include <common/xf_headers.hpp>
#include <common/xf_axi.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/utility.hpp>
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

/**
 * The maximum allowed difference between hw and sw result rho
 */
#define MAX_DIFF_RHO		(2.0f * TEST_MAX_DIFF)

/**
 * The maximum allowed difference between hw and sw result theta
 */
#define MAX_DIFF_THETA		(2.0f * VPU_FX_HOUGH_RES)

/**
 * The amount of dimensions the array of results has
 */
#define AXIS_DIMS		(2)

/**
 * The total amount of bits the fixed point type has
 */
#define AXIS_FP_WIDTH		(32)

/**
 * The resolution bits of the fixed point type
 */
#define AXIS_FP_FRAC		(22)

/**
 * @block
 *
 * Exit code for when certain tests fail
 */
#define EXIT_FAILURE_READ	(EXIT_FAILURE + 0)
#define EXIT_FAILURE_EMPTY	(EXIT_FAILURE + 1)
#define EXIT_FAILURE_HW_LESS	(EXIT_FAILURE + 2)
#define EXIT_FAILURE_INTEGRITY	(EXIT_FAILURE + 3)
#define EXIT_FAILURE_OPENCL	(EXIT_FAILURE + 4)

/**
 * Apply the filters on the host machine using OpenCV to create a reference image
 *
 * @param src	 The source image to use
 * @param result Where to place the resulting lines
 *
 * @tparam M	 Which opencv data type to use for storing the image
 * 		 (Mat for CPU-only processing and UMat for TAPI multi-
 * 		  device processing)
 */
template<typename M>
static void apply_ocv(M &src, std::vector<cv::Vec2f> &result) {
	/**
	 * Which size to use for the theta step, which angles to check
	 */
	constexpr double theta_resolution = CV_PI / 180 * VPU_FX_HOUGH_RES;
	/**
	 * The intermediate processed images
	 */
	M hsv, seg, gauss, sobel, edges;

	// Allocate space for the intermediate results
	hsv.create(src.rows, src.cols, CV_8UC3);
	seg.create(src.rows, src.cols, CV_8UC1);
	gauss.create(src.rows, src.cols, CV_8UC3);
	sobel.create(src.rows, src.cols, CV_8UC3);
	edges.create(src.rows, src.cols, CV_8UC3);

	// Convert source image from BGR to HSV
	cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);
	
	// Threshold the white values
	cv::inRange(hsv, cv::Scalar(0, 0, 255-ARG_SEG_THRESH), cv::Scalar(255, ARG_SEG_THRESH, 255), seg);

	// Apply a slight Gaussian blur before we run the Sobel operator
	cv::GaussianBlur(seg, gauss, cv::Size(VPU_FX_GAUSS_SIZE, VPU_FX_GAUSS_SIZE), ARG_GAUSS_SIGMA, ARG_GAUSS_SIGMA, cv::BORDER_CONSTANT);

	// Highlight edges using the Sobel operator
	cv::Sobel(gauss, sobel, CV_8UC1, 1, 1, 3, 1, 0, cv::BORDER_CONSTANT);

	// Threshold the Sobel results to a binary image
	cv::threshold(sobel, edges, ARG_EDGE_THRESH, 255, cv::THRESH_BINARY);

	// Detect the road lane markings
	cv::HoughLines(edges, result, 1.0d, theta_resolution, ARG_HOUGH_THRESH);

	// Release the allocated memory
	hsv.release();
	seg.release();
	gauss.release();
	sobel.release();
	edges.release();
}

/**
 * Read a 2d float array from a software-emulated HLS stream in fixed point format<br />
 * <br />
 * The fixed point format is described as Q<FP_W-FP_F>.<FP_F> in TI format
 *
 * @param stream  The stream to read from
 * @param results Where to place the results (pre allocated destination)
 *
 * @tparam N 	  The width of the results array
 * @tparam D	  The depth of the results array
 * @tparam FP_W	  The number of bits to use for the fixed point format
 * @tparam FP_F	  The number of fraction bits to use for the FP format
 */
template<int N, int D, int FP_W, int FP_F>
static void fpread(hls::stream<ap_axis<FP_W, 1, 1, 1>> &stream, float results[D][N]) {
	/**
	 * The bitmask to use when dividing the fixed point number to a float
	 */
	constexpr int bitmask = (1 << (FP_F));
	ap_axis<FP_W, 1, 1, 1> elem;

	// This is host-only code
	#ifdef __SYNTHESIS__
	#error Not synthesizable, use on host only
	#endif

	// Fill array row-major first
	for (int d = 0; d < D; ++d) {
		for (int n = 0; n < N; ++n) {
			// Read the element from the stream
			stream.read(elem);

			// Convert it to float and write it to the output array
			results[d][n] = elem.data / bitmask;
		}
	}
}

/**
 * Run the image through the hardware kernel<br />
 * <br />
 * Depending if csim or cosim is used, Vitis workflow will adapt and
 * automatically choose if the executable runs in XSim or not
 *
 * @param src	  The source image
 * @param results Where to place the results (pre allocated destination)
 *
 * @tparam N 	  The width of the results array
 * @tparam D	  The depth of the results array
 * @tparam FP_W	  The number of bits to use for the fixed point format
 * @tparam FP_F	  The number of fraction bits to use for the FP format
 */
template<int N, int D, int FP_W, int FP_F>
static void apply_hw(cv::Mat &src, float results[D][N]) {
	hls_stream_t<VPU_IMAGE_INPUT_TYPE> hw_in_stream;
	hls::stream<ap_axis<FP_W, 1, 1, 1>> hw_out_stream;

	// Simulate the AXI stream input
	xf::cv::cvMat2AXIvideoxf<XF_NPPC1, VPU_IMAGE_INPUT_THT>(src, hw_in_stream);

	// Call the top function in CSIM
	vpu_accel_top(
		hw_in_stream, hw_out_stream,
		src.rows, src.cols,
		static_cast<uint8_t>(ARG_SEG_THRESH),
		ARG_GAUSS_SIGMA,
		static_cast<short>(ARG_EDGE_THRESH),
		static_cast<short>(ARG_HOUGH_THRESH)
	);

	// Read the results from the output stream
	fpread<N, D, FP_W, FP_F>(hw_out_stream, results);
}

/**
 * Entry point function which loads the image from disk and runs tests
 *
 * @param argc	Amount of command-line arguments + 1
 * @param argv	Array of command-line arguments
 *
 * @return	Exit code with the test status (see EXIT_* defs)
 */
int main(int argc, char **argv) {
	/**
	 * Image data for CPU-only operations
	 */
	cv::Mat			src;
	/**
	 * Image data for OpenCL (iGPU, CPU) operations
	 */
	cv::UMat		srcu;
	/**
	 * Elapsed time measuring utility
	 */
	cv::TickMeter		tm;
	/**
	 * Host-generated results locations
	 */
	std::vector<cv::Vec2f>	sw_results_cpu, sw_results_gpu;
	/**
	 * Simulated hardware results location
	 */
	float			hw_results[AXIS_DIMS][ARG_MAXLINES];

	// Check if user supplied a path argument when running the executable
	if (argc != 2) {
		std::cerr << "Provide an image path as argument!" << std::endl;
		return EXIT_FAILURE_READ;
	}

	// Check if OpenCL is available
	if (cv::ocl::haveOpenCL()) {
		std::cerr << "No opencl available" << std::endl;
		return EXIT_FAILURE_OPENCL;
	}

	// Load the image from disk
	src = cv::imread(argv[1], cv::IMREAD_COLOR);

	// Check if the image is loaded correctly by measuring if it has data
	if (src.empty()) {
		std::cerr << "Failed to read image: " << argv[1] << std::endl;
		return EXIT_FAILURE_EMPTY;
	}

	// Copy the image data to VRAM
	src.copyTo(srcu);
	std::cout << "Image from: " << argv[1]  << " with res " << src.cols << "x" << src.rows << " px" << std::endl;

	std::cout << "Creating opencv reference using CPU..." << std::endl;
	cv::ocl::setUseOpenCL(false);
	tm.reset();
	tm.start();
	apply_ocv<cv::Mat>(src, sw_results_cpu);
	tm.stop();
	std::cout << "... done in " << tm.getTimeMilli() << " ms" << std::endl;

	std::cout << "Creating opencv reference using GPU..." << std::endl;
	cv::ocl::setUseOpenCL(true);
	tm.reset();
	tm.start();
	apply_ocv<cv::UMat>(srcu, sw_results_gpu);
	tm.stop();
	std::cout << "... done in " << tm.getTimeMilli() << " ms" << std::endl;

	std::cout << "Running hardware kernel..." << std::endl;
	apply_hw<ARG_MAXLINES, AXIS_DIMS, AXIS_FP_WIDTH, AXIS_FP_FRAC>(src, hw_results);

	std::cout << "sw results len: " << sw_results_cpu.size() << ".." << std::endl;

	// Check if all results match
	for (int i = 0; i < std::min(static_cast<int>(sw_results_cpu.size()), ARG_MAXLINES); ++i) {
		float	hw_rho = hw_results[0][i],
			hw_theta = hw_results[1][i],
			sw_rho = sw_results_cpu[i][0],
			sw_theta = sw_results_cpu[i][1],
			diff_rho = std::fabs(hw_rho - sw_rho),
			diff_theta = std::fabs(hw_theta - sw_theta);

		// Duplicate result means EOF
		if (i != 0 && (hw_rho == hw_results[0][i-1])
			   || (hw_theta == hw_results[1][i-1])) {
			return EXIT_FAILURE_HW_LESS;
		}

		std::cout << "rho: " << sw_results_cpu[i][0]
			  << " th: " << sw_results_cpu[i][1] << std::endl;

		// Fail if the difference is not within the allowed margin
		if (diff_rho > MAX_DIFF_RHO || diff_theta > MAX_DIFF_THETA) {
			return EXIT_FAILURE_INTEGRITY;
		}
	}
	
	// Deallocate image data
	src.release();
	srcu.release();

	return EXIT_SUCCESS;
}

