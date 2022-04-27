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
template<typename T, XF_npt_e BPP>
void vpu_stream_read(hls_stream_t<T> &stream, img_mat_t<BPP> &mat) {
	hls_pixel_t<T> pixel;
	int rows = mat.rows;
	int cols = mat.cols >> XF_BITSHIFT(VPU_IMAGE_PPC);

l_rows:	for (int i = 0; i < rows; ++i) {
l_cols:		for (int j = 0; j < cols; ++j) {
			#pragma HLS loop_flatten off
			#pragma HLS pipeline II=1

			stream.read(pixel);
			mat.write(i*rows + j, pixel.data(VPU_IMAGE_INPUT_THT - 1, 0));
		}
	}
}

/*
 * @inheritDoc
 */
template<typename T, XF_npt_e BPP>
void vpu_stream_write(img_mat_t<BPP> &mat, hls_stream_t<T> &stream) {
	hls_pixel_t<T> pixel;
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
			pixel.data(VPU_IMAGE_OUTPUT_THT - 1, 0) = mat.read(i*rows + j);
			pixel.keep = -1;
			stream.write(pixel);
		}
	}
}

/*
 * @inheritDoc
 */
template<unsigned int ROWS, unsigned int COLS>
void vpu_stream_fwrite(float data[ROWS][COLS], hls::stream<ap_axiu<32, 1, 1, 1>> &stream) {
	ap_axiu<32, 1, 1, 1> elem;
	int rows = ROWS;
	int cols = COLS >> XF_BITSHIFT(XF_NPPC1);
	
l_row:	for (int i = 0; i < rows; ++i) {
l_col:		for (int j = 0; j < cols; ++j) {
			#pragma HLS loop_flatten off
			#pragma HLS pipeline II=1

			// pixel.last = (j == cols-1) && (i == rows-1);
			if ((j == cols-1) && (i == rows-1)) {
				elem.last = 1;
			} else {
				elem.last = 0;
			}

			elem.data = 0;
			elem.data((32 * XF_NPIXPERCYCLE(XF_NPPC1)) - 1, 0) = data[i][j];
			elem.keep = -1;
			stream.write(elem);
		}
	}
}
