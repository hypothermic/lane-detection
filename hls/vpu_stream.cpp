/**
 * @file vpu_stream.cpp
 * @author Matthijs Bakker
 * @brief Read/write xf::cv::Mat from/to AXI streams
 *
 * This unit provides utilities for reading image matrices from AXI Streams
 * and writing image matrices to AXI Streams
 */

#include "vpu_stream.hpp"

/*
 * @inheritDoc
 */
void vpu_stream_read(hls_stream_t &stream, img_mat_t &mat) {
	hls_pixel_t pixel;
	int rows = mat.rows;
	int cols = mat.cols >> XF_BITSHIFT(VPU_IMAGE_PPC);

l_rows:	for (int i = 0; i < rows; ++i) {
l_cols:		for (int j = 0; j < cols; ++j) {
			#pragma HLS loop_flatten off
			#pragma HLS pipeline II=1

			stream.read(pixel);
			mat.write(i*rows + j, pixel.data(VPU_IMAGE_THT - 1, 0));
		}
	}
}

/*
 * @inheritDoc
 */
void vpu_stream_write(img_mat_t &mat, hls_stream_t &stream) {
	hls_pixel_t pixel;
	int rows = mat.rows;
	int cols = mat.cols >> XF_BITSHIFT(VPU_IMAGE_PPC);
	
l_row:	for (int i = 0; i < rows; ++i) {
l_col:		for (int j = 0; j < cols; ++j) {
			#pragma HLS loop_flatten off
			#pragma HLS pipeline II=1

			// pixel.last = (j == cols-1) && (i == rows-1);
			if ((j == cols-1) && (i == rows-1)) {
				pixel.last = 1;
			} else {
				pixel.last = 0;
			}

			pixel.data = 0;
			pixel.data(VPU_IMAGE_THT - 1, 0) = mat.read(i*rows + j);
			pixel.keep = -1;
			stream.write(pixel);
		}
	}
}

