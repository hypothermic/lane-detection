/**
 * @file vpu_accel.hpp
 * @author Matthijs Bakker
 * @brief Accelerated video processing function
 *
 * HLS-Synthesizable function for our Lane Detection algorithm.
 * This code gets unrolled and synthesized by the magic program called Vitis.
 */

#ifndef VPU_ACCEL_HPP
#define VPU_ACCEL_HPP

#include <hls_stream.h>
#include <common/xf_common.hpp>
#include <common/xf_infra.hpp>
#include <imgproc/xf_resize.hpp>

/**
 * Image width in pixels.
 */
#define VPU_IMAGE_WIDTH		(3840)

/**
 * Image height in pixels.
 */
#define VPU_IMAGE_HEIGHT	(2160)

/**
 * Bits per pixel of the image.
 *
 * We use RGB888 so that amount to 24 bits.
 */
#define VPU_IMAGE_BPP		(24)

/**
 * Pixels per cycle.
 */
#define VPU_IMAGE_PPC		(XF_NPPC1)

/**
 * Data throughput of the AXI Stream.
 *
 * Total number of bits per AXI Transfer.
 */
#define VPU_IMAGE_THT		(VPU_IMAGE_BPP * XF_NPIXPERCYCLE(VPU_IMAGE_PPC))
/**
 * The data type for a pixel
 */
typedef ap_axiu<VPU_IMAGE_BPP, 1, 1, 1> hls_pixel_t;

/**
 * The data type for an HLS stream
 */
typedef hls::stream<hls_pixel_t> hls_stream_t;

typedef xf::cv::Mat<XF_8UC3, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, XF_NPPC1> img_mat_t;

/**
 * Accelerator function that performs a Sobel filter on an image
 *
 * @param in		Input image row data
 * @param out		Output image row data
 * @param row		Which row is currently being processed
 * @param col		Which column is currently being processed
 */
void vpu_accel_top(hls_stream_t &in, hls_stream_t &out, int in_height, int in_width, int out_height, int out_width);

#endif /* VPU_ACCEL_HPP */

