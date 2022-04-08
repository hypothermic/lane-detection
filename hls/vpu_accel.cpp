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
	#pragma HLS interface axis register both port=in
	#pragma HLS interface axis register both port=out

	#pragma HLS interface s_axilite port=in_height
	#pragma HLS interface s_axilite port=in_width
	#pragma HLS interface s_axilite port=return

	img_mat_t<VPU_IMAGE_INPUT_BPP>	input(in_height, in_width);
	img_mat_t<VPU_IMAGE_OUTPUT_BPP>	intermediate(in_height, in_width),
					dstmatx(in_height, in_width),
					dstmaty(in_height, in_width),
					sobel(in_height, in_width),
					output(in_height, in_width);

	#pragma HLS dataflow

	vpu_stream_read<VPU_IMAGE_INPUT_TYPE, VPU_IMAGE_INPUT_BPP>(in, input);

	xf::cv::bgr2gray<VPU_IMAGE_INPUT_BPP, VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, VPU_IMAGE_PPC>(input, intermediate);
	
	xf::cv::Sobel<XF_BORDER_CONSTANT, XF_FILTER_3X3, VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, VPU_IMAGE_PPC, false>(intermediate, dstmatx, dstmaty);
	xf::cv::addWeighted<VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, VPU_IMAGE_PPC>(dstmatx, 0.5, dstmaty, 0.5, 0.0, sobel);

	xf::cv::Threshold<XF_THRESHOLD_TYPE_BINARY, XF_8UC1, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, VPU_IMAGE_PPC>(sobel, output, 127, 255);

	vpu_stream_write<VPU_IMAGE_OUTPUT_TYPE, VPU_IMAGE_OUTPUT_BPP>(output, out);
}

