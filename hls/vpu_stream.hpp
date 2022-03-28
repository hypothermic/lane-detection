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

/**
 * Reads an image from an AXI Stream
 *
 * @param stream	The AXI Stream to read the image from
 * @param mat		The matrix to save image data in
 */
void vpu_stream_read(hls_stream_t &stream, img_mat_t &mat);

/**
 * Writes an image to an AXI Stream
 *
 * @param mat		The image matrix to read from
 * @param stream	The AXI Stream to write the image to
 */
void vpu_stream_write(img_mat_t &mat, hls_stream_t &stream);

#endif /* VPU_STREAM_HPP */

