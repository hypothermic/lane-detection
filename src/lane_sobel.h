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

#include <stdbool.h>

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
 * @param magnitudes	The output image, which will be (over)written to
 * @param directions	The gradient directions for each pixel
 */
void lane_sobel_apply(const lane_image_t *const src, lane_image_t **magnitudes, double **directions);

/**
 * @brief Apply non-maximum suppression
 *
 * Discard unwanted pixels which may not constitude edges.<br />
 * <br />
 * This function only keeps the local maxima pixel in each cluster
 * of related edge pixels.<br />
 * <b>Note:</b> The output image will be cropped by <i>(3-1)*2</i> px
 * because there is no data to apply the kernel on in the borders of
 * the image!<br />
 * <b>Note:</b> It is assumed that the input image is already in
 * grayscale, so the color values for R,G,B match for every pixel.
 *
 * @param src		The input image with edge values
 * @param directions	The gradient directions for each pixel
 * 			encoded in a row-major array
 * @param dest		Where the output image should be placed
 */
void lane_nonmax_apply(const lane_image_t *const src, const double *const directions, lane_image_t **dest);

/**
 * @brief Apply edge tracking by hysteresis
 *
 * Track if weak edges are connected to strong edges.<br />
 * <br />
 * If they are not connected, they will be discarded.
 *
 * @param image		The input image with weak and strong edges
 * @param weak		The value of weak edges
 * @param strong	The value of strong edges
 */
void lane_hysteresis_apply(lane_image_t *image, uint8_t weak, uint8_t strong);

#endif /* LANE_SOBEL_H */
