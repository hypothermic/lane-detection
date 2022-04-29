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

#include <limits>
#include <hls_stream.h>

#include <common/xf_common.hpp>
#include <common/xf_infra.hpp>
#include <common/xf_types.hpp>
#include <common/xf_utility.hpp>

/**
 * The maximum image width that will be supported by the hardware.<br />
 * <br />
 * A larger width will mean a larger core that will take up a
 * greater amount of FPGA resources.<br />
 * <br />
 * The input image width must be smaller or equal to this number
 * and can be set/modified at runtime via the AXI-Lite registers.
 */
#define VPU_IMAGE_MAX_WIDTH	(1280)

/**
 * The maximum image height that will be supported by the hardware.<br />
 * <br />
 * A larger height will mean a larger core that will take up a
 * greater amount of FPGA resources.<br />
 * <br />
 * The input image height must be smaller or equal to this number
 * and can be set/modified at runtime via the AXI-Lite registers.
 */
#define VPU_IMAGE_MAX_HEIGHT	(720)

/**
 * How many lines the Hough transform can pick up.<br />
 * <br />
 * The most voted lines are always returned first.
 */
#define VPU_IMAGE_MAX_HOUGH	(32)

/**
 * The amount of pixels to process per cycle.<br />
 * <br />
 * This is useful for creating area optimized solutions but we
 * don't really care because we don't reuse logic...
 */
#define VPU_IMAGE_PPC		(XF_NPPC1)

/**
 * Channel configuration of the input video.<br />
 * <br />
 * We use RGB888 so we have three 8-bit channels.
 */
#define VPU_IMAGE_INPUT_NPT	(XF_8UC3)

/**
 * Channel configuration of the intermediate images.<br />
 * <br />
 * This is also used for the edge images because there is no smaller
 * data type available in Vitis Vision (XF_2UC1 must be packed!)
 */
#define VPU_IMAGE_GRAY_NPT	(XF_8UC1)

/**
 * Pixel width of the input image stream.<br />
 * <br />
 * This should be the same as the BPP since we're using 1ppc
 */
#define VPU_IMAGE_INPUT_WIDTH	(VPU_IMAGE_INPUT_TYPE::pixelwidth)

/**
 * Pixel width of the intermediate images.<br />
 * <br />
 * This should be the same as the BPP since we're using 1ppc
 */
#define VPU_IMAGE_GRAY_WIDTH	(VPU_IMAGE_OUTPUT_TYPE::pixelwidth)

/**
 * Output stream element width
 */
#define VPU_IMAGE_OUTPUT_WIDTH	(VPU_FP_WIDTH)

/**
 * The DataType structure of the input image.
 */
#define VPU_IMAGE_INPUT_TYPE	DataType<VPU_IMAGE_INPUT_NPT, VPU_IMAGE_PPC>

/**
 * The DataType structure of the output image.
 */
#define VPU_IMAGE_GRAY_TYPE	DataType<VPU_IMAGE_GRAY_NPT, VPU_IMAGE_PPC>

/**
 * Data throughput of the inbound AXI Stream.<br />
 * <br />
 * Total number of bits per AXI Transfer.
 */
#define VPU_IMAGE_INPUT_THT	(VPU_IMAGE_INPUT_WIDTH * XF_NPIXPERCYCLE(VPU_IMAGE_PPC))

/**
 * Data throughput of the outbound AXI Stream.<br />
 * <br />
 * Total number of bits per AXI Transfer.
 */
#define VPU_IMAGE_OUTPUT_THT	(VPU_IMAGE_OUTPUT_WIDTH * XF_NPIXPERCYCLE(VPU_IMAGE_PPC))

/**
 * The primitive type to use as a type for the fixed point representation
 */
#define VPU_FP_TYPE		int32_t

/**
 * Amount of fractional bits to use, aka decimal precision.<br />
 * <br />
 * We use Q10.22 so we have 22 fractional bits.<br />
 * <br />
 * Technically it's Q9.22 since the sign bit is present but
 * it seems like AMD includes the sign bit in the notation.,,
 */
#define VPU_FP_FRAC		(22)

/**
 * The width of the fixed point wrapper type in bits.
 */
// either i'm doing something wrong or gcc is stupid and won't eval this
//#define VPU_FP_WIDTH		(8 * sizeof(VPU_FP_TYPE))
constexpr int VPU_FP_WIDTH =	8 * sizeof(VPU_FP_TYPE);

/**
 * The size of the Gaussian kernel used for blurring the
 * image in the preprocessing stage.
 */
#define VPU_FX_GAUSS_SIZE	(5)

/**
 * The size of the Sobel kernels used for edge detection
 * of the image in the preprocessing stage.
 */
#define VPU_FX_SOBEL_SIZE	(3)

/**
 * How much the results of the individual sobel kernel
 * applications should weigh.<br />
 * <br />
 * We have 2 kernels: horizontal and vertical, so 1/2.
 */
#define VPU_FX_SOBEL_WEIGHT	(0.5f)

/**
 * How much to add/subtract from the sobel merged result.
 */
#define VPU_FX_SOBEL_GAMMA	(0.0f)

/**
 * Minimum theta value to use for the Hough transform.
 */
#define VPU_FX_HOUGH_ANGLE_MIN	(0)

/**
 * Maximum theta value to use for the Hough transform.
 */
#define VPU_FX_HOUGH_ANGLE_MAX	(180)

/**
 * Rho resolution to use for the Hough transform
 */
#define VPU_FX_HOUGH_RHO	(1)

/**
 * Theta resolution to use for the Hough transform<br />
 * <br />
 * Note that this is written in Q6.1 form
 */
#define VPU_FX_HOUGH_RES	(4) // 2.0

/**
 * Whether to use UltraRAM or regular BRAM/DRAM.<br />
 * <br />
 * This type is not available on our Z2 but more modern
 * (read: expensive) FPGAs have this, so it may be
 * useful in the future.
 */
#define VPU_USE_URAM		(false)

/**
 * Use the standard vitis AXI<->xfMat when simulating/cosimming
 */
#ifdef __XFSYNTHESIS__
#define __VITIS_HLS_PHASE_CSIM__
#endif

/**
 * @internal
 *
 * The data type for a pixel
 */
//typedef ap_axiu<T::pixelwidth, 1, 1, 1> hls_pixel_t;
template<typename T> // T = VPU_IMAGE_*_TYPE
using hls_pixel_t = ap_axiu<T::pixelwidth, 1, 1, 1>;

/**
 * @internal
 *
 * The data type for an HLS stream
 */
//typedef hls::stream<hls_pixel_t> hls_stream_t;
template<typename T> // T = VPU_IMAGE_*_TYPE
using hls_stream_t = hls::stream<hls_pixel_t<T>>;

/**
 * @internal
 *
 * The data type for an image
 */
//typedef xf::cv::Mat<XF_8UC3, VPU_IMAGE_HEIGHT, VPU_IMAGE_WIDTH, XF_NPPC1> img_mat_t;
template<XF_npt_e NPT> // NPT = XF_npt_e (XF_*UC*)
using img_mat_t = xf::cv::Mat<NPT, VPU_IMAGE_MAX_HEIGHT, VPU_IMAGE_MAX_WIDTH, VPU_IMAGE_PPC>;

#endif /* VPU_TYPE_HPP */

