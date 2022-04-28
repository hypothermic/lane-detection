/**
 * @file vpu_accel.cpp
 * @author Matthijs Bakker
 * @brief Accelerated video processing function
 */

#include "vpu_accel.hpp"
#include "vpu_stream.hpp"

// For some reason, Vitis refuses to properly link object files
// so instead of linking code units at compile time I just
// include the functions from other files directly into this file
#include "vpu_stream.cpp" // bad practice btw

#define HOUGH_MAXLINES	(32)
#define FP_WIDTH	(32) //Total FP bits
#define FP_SHIFT	(FP_WIDTH-10) // Q10.22

void vpu_stream_fpwrite(hls::stream<ap_axis<32, 1, 1, 1>> &out, float rhos[HOUGH_MAXLINES], float thetas[HOUGH_MAXLINES]) {
	ap_axis<32, 1, 1, 1>	elem;

l_row:	for (int i = 0; i < 2; ++i) {
l_col:		for (int j = 0; j < HOUGH_MAXLINES; ++j) {
			#pragma HLS loop_flatten off
			#pragma HLS pipeline II=1

			// pixel.last = (j == cols-1) && (i == rows-1);
			if ((j == HOUGH_MAXLINES-1) && (i == 2-1)) {
				elem.last = 1;
			} else {
				elem.last = 0;
			}

			elem.data = 0;
			//elem.data(32-1, 0) = (uint32_t)((1.1f * (i*8.0f + j + (0.0000000000000000000000000000001f * rhos[j] * thetas[j]))) * (1 << 16));
			if (i == 2-1) {
				elem.data(32-1, 0) = (int32_t)(thetas[j] * (1 << FP_SHIFT));
			} else {
				elem.data(32-1, 0) = (int32_t)(rhos[j] * (1 << FP_SHIFT));
			}
			elem.keep = -1;
			out.write(elem);
		}
	}
}

/*
 * @inheritDoc
 */
void vpu_accel_top(hls::stream<ap_axiu<24, 1, 1, 1>> &in, hls::stream<ap_axis<32, 1, 1, 1>> &out, int in_height, int in_width, uint8_t seg_thres, float g_sigma, short e_thres, short h_thres) {
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

	img_mat_t<XF_8UC3>		img_input(in_height, in_width),
					img_hsv(in_height, in_width);
	img_mat_t<XF_8UC1>		img_seg(in_height, in_width),
					img_gauss(in_height, in_width),
					img_sobx(in_height, in_width),
					img_soby(in_height, in_width),
					img_sob(in_height, in_width),
					img_edges(in_height, in_width);
	float				rhos[HOUGH_MAXLINES], thetas[HOUGH_MAXLINES];

	// clang-format off
	#pragma HLS dataflow
	// clang-format on
	
	unsigned char seg_thresh_low[XF_CHANNELS(XF_8UC3, XF_NPPC1)] = {0, 0, static_cast<unsigned char>(255-seg_thres)};
	unsigned char seg_thresh_high[XF_CHANNELS(XF_8UC3, XF_NPPC1)] = {255, static_cast<unsigned char>(seg_thres), 255};

//	#ifdef __VITIS_HLS_PHASE_CSIM__
//		xf::cv::AXIvideo2xfMat(in, input);
//	#else
		vpu_stream_read<VPU_IMAGE_INPUT_TYPE, XF_8UC3>(in, img_input);
//	#endif

	xf::cv::bgr2hsv<XF_8UC3, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, XF_NPPC1>(img_input, img_hsv);
	xf::cv::inRange<XF_8UC3, XF_8UC1, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, XF_NPPC1>(img_hsv, seg_thresh_low, seg_thresh_high, img_seg);
	xf::cv::GaussianBlur<5, XF_BORDER_CONSTANT, XF_8UC1, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, XF_NPPC1>(img_seg, img_gauss, g_sigma);
	xf::cv::Sobel<XF_BORDER_CONSTANT, 3, XF_8UC1, XF_8UC1, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, XF_NPPC1, false>(img_gauss, img_sobx, img_soby);
	xf::cv::addWeighted<XF_8UC1, XF_8UC1, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, XF_NPPC1>(img_sobx, 0.5, img_soby, 0.5, 0.0, img_sob);
	xf::cv::Threshold<XF_THRESHOLD_TYPE_BINARY, XF_8UC1, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, XF_NPPC1>(img_sob, img_edges, e_thres, 255);
	xf::cv::HoughLines<1, 4, HOUGH_MAXLINES, 1469, 0, 180, XF_8UC1, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, XF_NPPC1>(img_edges, rhos, thetas, h_thres, HOUGH_MAXLINES);

	vpu_stream_fpwrite(out, rhos, thetas);
}

