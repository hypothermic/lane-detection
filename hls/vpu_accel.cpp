/**
 * @file vpu_accel.cpp
 * @author Matthijs Bakker
 * @brief Accelerated video processing function
 */

#include "vpu_accel.hpp"
#include "vpu_stream.hpp"
#include "vpu_util.hpp"

// For some reason, Vitis refuses to properly link object files
// so instead of linking code units at compile time I just
// include the functions from other files directly into this file
#include "vpu_stream.cpp" // bad practice btw

/*
 * @inheritDoc
 */
void vpu_accel_top(hls::stream<ap_axiu<VPU_IMAGE_INPUT_WIDTH, 1, 1, 1>> &in, hls::stream<ap_axis<VPU_IMAGE_OUTPUT_WIDTH, 1, 1, 1>> &out, int in_height, int in_width, uint8_t seg_thres, float g_sigma, short e_thres, short h_thres) {
	// clang-format off
	#pragma HLS interface axis register both port=in
	#pragma HLS interface axis register both port=out

	#pragma HLS interface s_axilite port=in_height
	#pragma HLS interface s_axilite port=in_width
	#pragma HLS interface s_axilite port=seg_thres
	#pragma HLS interface s_axilite port=g_sigma
	#pragma HLS interface s_axilite port=e_thres
	#pragma HLS interface s_axilite port=h_thres
	#pragma HLS interface s_axilite port=return
	// clang-format on

	// Min/max values for the thresholds
	constexpr unsigned char ch_min = std::numeric_limits<unsigned char>::min(),
		  		ch_max = std::numeric_limits<unsigned char>::max();

	// Image diagonal for the Hough transform and parameter space
	constexpr int		hough_diag = (int)(vpu_sqrt(
					vpu_apow(VPU_IMAGE_MAX_WIDTH, 2) +
					vpu_apow(VPU_IMAGE_MAX_HEIGHT, 2)
				) / VPU_FX_HOUGH_RHO);

	// Full color input images
	img_mat_t<XF_8UC3>	img_input(in_height, in_width),
				img_hsv(in_height, in_width);

	// Intermediate binary segmented and grayscale images
	img_mat_t<XF_8UC1>	img_seg(in_height, in_width),
				img_gauss(in_height, in_width),
				img_sobx(in_height, in_width),
				img_soby(in_height, in_width),
				img_sob(in_height, in_width),
				img_edges(in_height, in_width);

	// HSV inRange thresholds
	unsigned char		seg_thresh_low[XF_CHANNELS(VPU_IMAGE_INPUT_NPT, VPU_IMAGE_PPC)]
					= {ch_min, ch_min, static_cast<unsigned char>(ch_max-seg_thres)},
				seg_thresh_high[XF_CHANNELS(VPU_IMAGE_INPUT_NPT, VPU_IMAGE_PPC)]
					= {ch_max, static_cast<unsigned char>(seg_thres), ch_max};

	// Output line coordinates of Hough transform
	float			rhos[VPU_IMAGE_MAX_HOUGH],
				thetas[VPU_IMAGE_MAX_HOUGH];

	// clang-format off
	#pragma HLS dataflow
	// clang-format on

	// Make sure to use the standard AXI<->xfMat for sim/cosim
	// because ours doesn't set the relevant channel signals
	#ifdef __VITIS_HLS_PHASE_CSIM__
		xf::cv::AXIvideo2xfMat(in, input);
	#else
		vpu_stream_read<VPU_IMAGE_INPUT_TYPE, XF_8UC3>(in, img_input);
	#endif

	// These functions need so many template arguments... it's like the Eigen library
	
	// Convert picture from BGR to HSV colorspace for easy segmentation
	xf::cv::bgr2hsv<VPU_IMAGE_INPUT_NPT, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, VPU_IMAGE_PPC>(img_input, img_hsv);

	// Segment the image using saturation and value based thresholding
	xf::cv::inRange<VPU_IMAGE_INPUT_NPT, VPU_IMAGE_GRAY_NPT, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, VPU_IMAGE_PPC>(img_hsv, seg_thresh_low, seg_thresh_high, img_seg);

	// Apply Gaussian blur to smooth the image before applying the Sobel operator
	xf::cv::GaussianBlur<VPU_FX_GAUSS_SIZE, XF_BORDER_CONSTANT, VPU_IMAGE_GRAY_NPT, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, VPU_IMAGE_PPC>(img_seg, img_gauss, g_sigma);

	// Apply the Sobel operator to highlight the edges in the image
	xf::cv::Sobel<XF_BORDER_CONSTANT, VPU_FX_SOBEL_SIZE, VPU_IMAGE_GRAY_NPT, VPU_IMAGE_GRAY_NPT, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, VPU_IMAGE_PPC, VPU_USE_URAM>(img_gauss, img_sobx, img_soby);

	// The Sobel function gives us the horizontal and vertical convolution results separately, merge them
	xf::cv::addWeighted<VPU_IMAGE_GRAY_NPT, VPU_IMAGE_GRAY_NPT, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, VPU_IMAGE_PPC>(img_sobx, VPU_FX_SOBEL_WEIGHT, img_soby, VPU_FX_SOBEL_WEIGHT, VPU_FX_SOBEL_GAMMA, img_sob);

	// Apply basic thresholding to the Sobel results to eliminate noise
	xf::cv::Threshold<XF_THRESHOLD_TYPE_BINARY, VPU_IMAGE_GRAY_NPT, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, VPU_IMAGE_PPC>(img_sob, img_edges, e_thres, ch_max);

	// Do the Hough Transform to detect lines in the image
	xf::cv::HoughLines<VPU_FX_HOUGH_RHO, VPU_FX_HOUGH_RES, VPU_IMAGE_MAX_HOUGH, 1469, VPU_FX_HOUGH_ANGLE_MIN, VPU_FX_HOUGH_ANGLE_MAX, XF_8UC1, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, VPU_IMAGE_PPC>(img_edges, rhos, thetas, h_thres, VPU_IMAGE_MAX_HOUGH);

	// Write the resulting polar line value pairs to the output stream
	vpu_stream_fpwrite<VPU_IMAGE_MAX_HOUGH, 2, VPU_FP_WIDTH, VPU_FP_FRAC>(out, rhos, thetas);
}

