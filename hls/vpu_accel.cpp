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

void _swrite(hls::stream<ap_axiu<32, 1, 1, 1>> &out, float rhos[8], float thetas[8]) {
	ap_axiu<32, 1, 1, 1>		elem;

l_row:	for (int i = 0; i < 2; ++i) {
l_col:		for (int j = 0; j < 8; ++j) {
			#pragma HLS loop_flatten off
			#pragma HLS pipeline II=1

			// pixel.last = (j == cols-1) && (i == rows-1);
			if ((j == 8-1) && (i == 2-1)) {
				elem.last = 1;
			} else {
				elem.last = 0;
			}

			elem.data = 0;
			//elem.data(32-1, 0) = (uint32_t)((1.1f * (i*8.0f + j + (0.0000000000000000000000000000001f * rhos[j] * thetas[j]))) * (1 << 16));
			if (i == 2-1) {
				elem.data(32-1, 0) = (uint32_t)(thetas[j] * (1 << 16));
			} else {
				elem.data(32-1, 0) = (uint32_t)(rhos[j] * (1 << 16));
			}
			elem.keep = -1;
			out.write(elem);
		}
	}
}

/*
 * @inheritDoc
 */
void vpu_accel_top(hls::stream<ap_axiu<24, 1, 1, 1>> &in, hls::stream<ap_axiu<32, 1, 1, 1>> &out, int in_height, int in_width, short thres) {
	// clang-format off
	#pragma HLS interface axis register both port=in
	#pragma HLS interface axis register both port=out

	#pragma HLS interface s_axilite port=in_height
	#pragma HLS interface s_axilite port=in_width
	#pragma HLS interface s_axilite port=thres
	#pragma HLS interface s_axilite port=return
	// clang-format on

	img_mat_t<XF_8UC1>		input(in_height, in_width);
	float				rhos[8], thetas[8];	

	// clang-format off
	#pragma HLS dataflow
	// clang-format on

//	#ifdef __VITIS_HLS_PHASE_CSIM__
//		xf::cv::AXIvideo2xfMat(in, input);
//	#else
		vpu_stream_read<VPU_IMAGE_INPUT_TYPE, XF_8UC1>(in, input);
//	#endif

	xf::cv::HoughLines<1, 2, 8, 1469, 0, 90, XF_8UC1, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, XF_NPPC1>(input, rhos, thetas, thres, 8);

	_swrite(out, rhos, thetas);
}

