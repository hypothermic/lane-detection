/**
 * @file lane_sobel.h
 * @author Matthijs Bakker
 * @brief Sobel filter for images
 *
 * This code unit provides a Sobel filter which convolutes
 * two predefined 3x3 kernels with an image and calculates
 * the gradient magnitude for each pixel.
 */

#ifndef LANE_SOBEL_H
#define LANE_SOBEL_H

#include "lane_image.h"

/**
 * @brief Highlight edges within an image using the Sobel operator
 *
 * Apply an iteration of the Sobel-Feldman operator to an image.<br />
 * <br />
 * The output image only consists of the gradient magnitudes, not
 * the gradient directions. <br />
 * <br />
 * <b>Note:</b> The output image will be cropped by <i>(3-1)*2</i> px
 * because there is no data to apply the kernel on in the borders of
 * the image!<br />
 * <b>Note:</b> It is assumed that the input image is already in
 * grayscale, so the color values for R,G,B match for every pixel.
 *
 * @param src		The input image, which data will be read
 * @param dest		The output image, which will be (over)written to
 */
void lane_sobel_apply(const lane_image_t *const src, lane_image_t **dest);

#endif /* LANE_SOBEL_H */
