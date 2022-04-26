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

/*
 * @inheritDoc
 */
void vpu_accel_top(hls_stream_t<VPU_IMAGE_INPUT_TYPE> &in, hls_stream_t<VPU_IMAGE_OUTPUT_TYPE> &out, int in_height, int in_width) {
	// clang-format off
	#pragma HLS interface axis register both port=in
	#pragma HLS interface axis register both port=out

	#pragma HLS interface s_axilite port=in_height
	#pragma HLS interface s_axilite port=in_width
	#pragma HLS interface s_axilite port=return
	// clang-format on

	img_mat_t<VPU_IMAGE_INPUT_BPP>	input(in_height, in_width);
	img_mat_t<VPU_IMAGE_OUTPUT_BPP>	intermediate(in_height, in_width),
					dstmatx(in_height, in_width),
					dstmaty(in_height, in_width),
					output(in_height, in_width);

	// clang-format off
	#pragma HLS dataflow
	// clang-format on

//	#ifdef __VITIS_HLS_PHASE_CSIM__
		xf::cv::AXIvideo2xfMat(in, input);
//	#else
//		vpu_stream_read<VPU_IMAGE_INPUT_TYPE, VPU_IMAGE_INPUT_BPP>(in, input);
//	#endif

	xf::cv::bgr2gray<VPU_IMAGE_INPUT_BPP, VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, VPU_IMAGE_PPC>(input, intermediate);
	
	xf::cv::Sobel<XF_BORDER_CONSTANT, XF_FILTER_3X3, VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, VPU_IMAGE_PPC, false>(intermediate, dstmatx, dstmaty);
	xf::cv::addWeighted<VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, VPU_IMAGE_PPC>(dstmatx, 0.5, dstmaty, 0.5, 0.0, output);

	/*
	 * For the hardware implementation we need to set TLAST ourselves,
	 * so we use our own function. But for simulation we can just use
	 * the provided function.
	 */
//	#ifdef __VITIS_HLS_PHASE_CSIM__
		xf::cv::xfMat2AXIvideo(output, out);
//	#else
//		vpu_stream_write<VPU_IMAGE_OUTPUT_TYPE, VPU_IMAGE_OUTPUT_BPP>(output, out);
//	#endif
}

