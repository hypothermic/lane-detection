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
 * @tparam T		The data type for the AXI stream elements
 * @tparam BPP		The number of pixels per clock cycle
 *
 * @param stream	The AXI Stream to read the image from
 * @param mat		The matrix to save image data in
 */
template<typename T, XF_npt_e BPP>
void vpu_stream_read(hls_stream_t<T> &stream, img_mat_t<BPP> &mat);

/**
 * Writes an image to an AXI Stream
 *
 * @tparam T		The data type for the AXI stream elements
 * @tparam BPP		The number of pixels per clock cycle
 *
 * @param mat		The image matrix to read from
 * @param stream	The AXI Stream to write the image to
 */
template<typename T, XF_npt_e BPP>
void vpu_stream_write(img_mat_t<BPP> &mat, hls_stream_t<T> &stream);

/**
 * Writes a bunch of floating point values over an AXI stream
 * by converting them to fixed point first
 *
 * @tparam N		The size of the float arrays
 * @tparam D		The amount of dimensions of the data
 * @tparam FP_W		The width of the fixed point type
 * @tparam FP_F		The amount of fractional bits to use
 *
 * @param stream	The AXI stream to write the values to
 * @param d1		First dimension of data
 * @param d2		Second dimension of data
 */
template<int N, int D, int FP_W, int FP_F>
void vpu_stream_fpwrite(hls::stream<ap_axis<FP_W, 1, 1, 1>> &stream, float d1[N], float d2[N]);

#endif /* VPU_STREAM_HPP */

