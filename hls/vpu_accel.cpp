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
					output(in_height, in_width);

	#pragma HLS dataflow

	vpu_stream_read<VPU_IMAGE_INPUT_TYPE, VPU_IMAGE_INPUT_BPP>(in, input);

	xf::cv::rgb2gray<VPU_IMAGE_INPUT_BPP, VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, VPU_IMAGE_PPC>(input, intermediate);
	
	xf::cv::medianBlur<5, XF_BORDER_REPLICATE, VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, VPU_IMAGE_PPC>(intermediate, output);
	
	vpu_stream_write<VPU_IMAGE_OUTPUT_TYPE, VPU_IMAGE_OUTPUT_BPP>(output, out);
}

