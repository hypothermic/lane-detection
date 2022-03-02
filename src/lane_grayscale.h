/**
 * @file lane_grayscale.h
 * @author Matthijs Bakker
 * @brief RGB to grayscale filter for images
 *
 * This code unit provides a filter for converting images
 * from the RGB colorspace to grayscale using the BT.601-7 weights.
 */

#ifndef LANE_GRAYSCALE_H
#define LANE_GRAYSCALE_H

#include "lane_image.h"

/**
 * Convert a color image to grayscale.<br />
 * <br />
 * The values of the color channels R,G,B are guaranteed to
 * be equal for each pixel in the resulting image.
 *
 * @param image	The input image, which contents will be
 * 		modified by this function
 */
void lane_grayscale_apply(lane_image_t *image);

#endif /* LANE_GRAYSCALE_H */
