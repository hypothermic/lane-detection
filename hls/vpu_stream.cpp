/**
 * @file vpu_stream.cpp
 * @author Matthijs Bakker
 * @brief Read/write xf::cv types from/to AXI streams
 *
 * This unit provides utilities for reading image matrices from AXI Streams
 * and writing image matrices to AXI Streams. The reason to use this instead
 * of the Xilinx-supplied xfDataMovers is the lack of data signaling in
 * their implementation.
 */

#include "vpu_stream.hpp"

/**
 * Shift mask for converting floats to fixed point
 */
#define FP_SHIFT		(1 << VPU_FP_FRAC)

/**
 * Macro for converting a floating point value to fixed point
 */
#define FP_CONVERT(afloat)	((VPU_FP_TYPE)(afloat * VPU_FP_SHIFT))

/*
 * @inheritDoc
 */
template<typename T, XF_npt_e BPP>
void vpu_stream_read(hls_stream_t<T> &stream, img_mat_t<BPP> &mat) {
	hls_pixel_t<T> pixel;
	int rows = mat.rows;
	int cols = mat.cols >> XF_BITSHIFT(VPU_IMAGE_PPC);

	// Rebuild image row-by-row
l_rows:	for (int i = 0; i < rows; ++i) {

		// Receive each pixel in a column
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

	// Loop vertically over image, we want to send a row at a time
l_row:	for (int i = 0; i < rows; ++i) {

		// Loop horizontally over image, send each pixel of a row
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
			pixel.data(/*VPU_IMAGE_OUTPUT_THT*/32 - 1, 0) = mat.read(i*rows + j);
			pixel.keep = -1;
			stream.write(pixel);
		}
	}
}

/*
 * @inheritDoc
 */
template<int N, int D, int FP_W = VPU_FP_WIDTH, int FP_F = VPU_FP_FRAC>
void vpu_stream_fpwrite(hls::stream<ap_axis<FP_W, 1, 1, 1>> &out, float rhos[N], float thetas[N]) {
	ap_axis<FP_W, 1, 1, 1> elem;

	// Elke iteratie van de binnenste loop duurt 9 klokcycles
	// maar hij is gepipelined met interval van 1 cycle
	// dus voor beide arrays zijn MINIMAAL 41 cycles nodig
	// anders is er iets fout gegaan bij de synth
	constexpr int MIN_LATENCY = D * (/*initiation*/9 + (N * /*interval*/1));
	#pragma HLS latency min=MIN_LATENCY

	// Loop over dimensions/pairs (p, 0)
l_dim:	for (int i = 0; i < D; ++i) {
		#pragma HLS dataflow
		#pragma HLS loop_tripcount min=D max=D

		// Loop over items in each dimension
l_iter:		for (int j = 0; j < N; ++j) {
			#pragma HLS loop_flatten off
			#pragma HLS pipeline II=1

			// The default Vitis-supplied AXI data movers
			// do not set the TLAST signal so we have to
			// set it ourselves to signal that the 
			// transaction is complete
			elem.last = (j == N-1) && (i == D-1);

			// Clear all data, even out of range
			// (though the AXI datawidth converter would
			// most likely take care of it otherwise)
			elem.data = 0;

			// Useful thing for testing if floats work correctly
			//elem.data(32-1, 0) = (uint32_t)((1.1f * (i*8.0f + j + (0.0000000000000000000000000000001f * rhos[j] * thetas[j]))) * (1 << 16));
			
			// Write the fixed point data to the stream element
			if (i == D-1) {
				elem.data(FP_W, 0) = (int32_t)(thetas[j] * (1 << FP_F));
			} else {
				elem.data(FP_W-1, 0) = (int32_t)(rhos[j] * (1 << FP_F));
			}

			// We don't use value signaling but
			// the ARM doc says it must be all high
			elem.keep = -1;

			out.write(elem);
		}
	}
}

