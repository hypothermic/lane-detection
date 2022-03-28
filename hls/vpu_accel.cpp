/**
 * @file vpu_accel.cpp
 * @author Matthijs Bakker
 * @brief Accelerated video processing function
 */

#include "vpu_accel.hpp"
#include "vpu_stream.hpp"

/*
 * @inheritDoc
 */
void vpu_accel_top(hls_stream_t &in, hls_stream_t &out, int in_height, int in_width, int out_height, int out_width) {
	#pragma HLS interface axis register both port=in
	#pragma HLS interface axis register both port=out

	#pragma HLS interface s_axilite port=in_height
	#pragma HLS interface s_axilite port=in_width
	#pragma HLS interface s_axilite port=out_height
	#pragma HLS interface s_axilite port=out_width

	img_mat_t input(in_height, in_width),
		  output(out_height, out_width);

	#pragma HLS dataflow

	vpu_stream_read(in, input);

	xf::cv::resize<XF_INTERPOLATION_BILINEAR, XF_8UC3, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, VPU_IMAGE_PPC, 9>(input, output);

	vpu_stream_write(output, out);
}

