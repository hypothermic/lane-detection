/**
 * @file vpu_stream.hpp
 * @author Matthijs Bakker
 * @brief Read/write xf::cv::Mat from/to AXI streams
 *
 * This unit provides utilities for reading image matrices from AXI Streams
 * and writing image matrices to AXI Streams
 */

#ifndef VPU_STREAM_HPP
#define VPU_STREAM_HPP

#include "vpu_accel.hpp"
#include "vpu_type.hpp"

/**
 * Reads an image from an AXI Stream
 *
 * @param stream	The AXI Stream to read the image from
 * @param mat		The matrix to save image data in
 */
template<typename T, XF_npt_e BPP>
void vpu_stream_read(hls_stream_t<T> &stream, img_mat_t<BPP> &mat);

/**
 * Writes an image to an AXI Stream
 *
 * @param mat		The image matrix to read from
 * @param stream	The AXI Stream to write the image to
 */
template<typename T, XF_npt_e BPP>
void vpu_stream_write(img_mat_t<BPP> &mat, hls_stream_t<T> &stream);

template<unsigned int ROWS, unsigned int COLS>
void vpu_stream_fwrite(float data[ROWS][COLS], hls::stream<ap_axiu<32, 1, 1, 1>> &stream);

#endif /* VPU_STREAM_HPP */

