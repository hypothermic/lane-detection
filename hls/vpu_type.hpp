/**
 * @file vpu_type.hpp
 * @author Matthijs Bakker
 * @brief Typedefs for easier working with HLS types
 *
 * This header file contains typedefs and predefined values
 * that simplify working with the Vitis Vision types.
 */

#ifndef VPU_TYPE_HPP
#define VPU_TYPE_HPP

#include <hls_stream.h>
#include <common/xf_common.hpp>
#include <common/xf_infra.hpp>
#include <common/xf_types.hpp>
#include <common/xf_utility.hpp>

/**
 * Image width in pixels.
 */
#define VPU_IMAGE_WIDTH		(1920)

/**
 * Image height in pixels.
 */
#define VPU_IMAGE_HEIGHT	(1080)

/**
 * Pixels per cycle.
 */
#define VPU_IMAGE_PPC		(XF_NPPC1)

/**
 * Bits per pixel of the input image.
 *
 * We use RGB888 so that amounts to 24 bits.
 */
#define VPU_IMAGE_INPUT_BPP	(XF_8UC3)

/**
 * Bits per pixel of the output image.
 *
 * We use grayscale, which amounts to 1 channel with 8 bits depth.
 */
#define VPU_IMAGE_OUTPUT_BPP	(XF_8UC1)

/**
 * The DataType structure of the input image
 */
#define VPU_IMAGE_INPUT_TYPE	DataType<VPU_IMAGE_INPUT_BPP, VPU_IMAGE_PPC>

/**
 * The DataType structure of the output image
 */
#define VPU_IMAGE_OUTPUT_TYPE	DataType<VPU_IMAGE_OUTPUT_BPP, VPU_IMAGE_PPC>

/**
 * Data throughput of the inbound AXI Stream.
 *
 * Total number of bits per AXI Transfer.
 */
#define VPU_IMAGE_INPUT_THT	(/*VPU_IMAGE_INPUT_BPP*/ 24 * XF_NPIXPERCYCLE(VPU_IMAGE_PPC))

/**
 * Data throughput of the outbound AXI Stream.
 *
 * Total number of bits per AXI Transfer.
 */
#define VPU_IMAGE_OUTPUT_THT	(/*VPU_IMAGE_OUTPUT_BPP*/ 8 * XF_NPIXPERCYCLE(VPU_IMAGE_PPC))

/**
 * The data type for a pixel
 */
//typedef ap_axiu<T::pixelwidth, 1, 1, 1> hls_pixel_t;
template<typename T> // T = VPU_IMAGE_*_TYPE
using hls_pixel_t = ap_axiu<T::pixelwidth, 1, 1, 1>;

/**
 * The data type for an HLS stream
 */
//typedef hls::stream<hls_pixel_t> hls_stream_t;
template<typename T> // T = VPU_IMAGE_*_TYPE
using hls_stream_t = hls::stream<hls_pixel_t<T>>;

/**
 * @internal
 */
//typedef xf::cv::Mat<XF_8UC3, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, XF_NPPC1> img_mat_t;
template<XF_npt_e BPP> // BPP = XF_npt_e (XF_*UC*)
using img_mat_t = xf::cv::Mat<BPP, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, VPU_IMAGE_PPC>;

#endif /* VPU_TYPE_HPP */