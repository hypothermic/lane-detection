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

#include <common/xf_structs.hpp>
#include <common/xf_infra.hpp>
#include <core/xf_arithm.hpp>

#include <imgproc/xf_add_weighted.hpp>
#include <imgproc/xf_bgr2hsv.hpp>
#include <imgproc/xf_cvt_color.hpp>
#include <imgproc/xf_gaussian_filter.hpp>
#include <imgproc/xf_houghlines.hpp>
#include <imgproc/xf_inrange.hpp>
#include <imgproc/xf_sobel.hpp>
#include <imgproc/xf_threshold.hpp>

#include "vpu_stream.hpp"
#include "vpu_type.hpp"

/**
 * Accelerator function that performs edge detection
 * and Hough transform to detect lines within an image
 *
 * @param in		Input image row data
 * @param out		Output image row data
 * @param row		Which row is currently being processed
 * @param col		Which column is currently being processed
 */
void vpu_accel_top(hls::stream<ap_axiu<VPU_IMAGE_INPUT_WIDTH, 1, 1, 1>> &in, hls::stream<ap_axis<VPU_IMAGE_OUTPUT_WIDTH, 1, 1, 1>> &out, int in_height, int in_width, uint8_t seg_thres, float g_sigma, short e_thres, short h_thres);

#endif /* VPU_ACCEL_HPP */

