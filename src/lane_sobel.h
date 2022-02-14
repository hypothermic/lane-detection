#ifndef LANE_SOBEL_H
#define LANE_SOBEL_H

#include "lane_image.h"

/**
 * Apply an iteration of the Sobel-Feldman operator to an image
 *
 * @param src		The input image, which data will be read
 * @param dest		The output image, which will be (over)written to
 */
void lane_sobel_apply(const lane_image_t *const src, lane_image_t **dest);

#endif /* LANE_SOBEL_H */
