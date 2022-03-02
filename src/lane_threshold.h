/**
 * @file lane_threshold.h
 * @author Matthijs Bakker
 * @brief Image thresholding for grayscale images
 *
 * This code unit provides a function to threshold monotone
 * images using upper and lower boundaries.
 */

#ifndef LANE_THRESHOLD_H
#define LANE_THRESHOLD_H

#include "lane_image.h"

/**
 * The default lower threshold.
 */
#define LANE_THRESHOLD_UNUSED_LOWER	0

/**
 * The default upper threshold.
 */
#define LANE_THRESHOLD_UNUSED_UPPER	255

/**
 * @brief Replace pixels with a value outside of a specific bound.
 *
 * Replace the pixels that don't fall within a certain boundary,
 * specified by the <i>upper</i> and <i>lower</i> arguments with
 * a specific value indicated with the <i>new</i> argument.
 *
 * @param image	The image that will be modified by this function
 * @param lower	The inclusive lower bound of the threshold (0 for unused)
 * @param upper	The inclusive upper bound of the threshold (255 for unused)
 * @param new	The replacement value when outside of threshold
 */
void lane_threshold_apply(lane_image_t *image, uint8_t lower, uint8_t upper, uint8_t new);

#endif /* LANE_THRESHOLD_H */
