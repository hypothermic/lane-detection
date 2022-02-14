#ifndef LANE_SOBEL_H
#define LANE_SOBEL_H

#include "lane_image.h"

/**
 * Apply an iteration of the Sobel-Feldman operator to an image
 *
 * @param image		The input image, which contents will be
 * 			modified by this function
 * @param threshold	The threshold to decide whether a pixel
 * 			should be discarded or not
 */
void lane_sobel_apply(const lane_image_t *const src, lane_image_t **dest, uint8_t threshold);

#endif /* LANE_SOBEL_H */
