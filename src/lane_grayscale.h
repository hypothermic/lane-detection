#ifndef LANE_GRAYSCALE_H
#define LANE_GRAYSCALE_H

#include "lane_image.h"

/**
 * Convert a color image to grayscale
 *
 * @param image	The input image, which contents will be
 * 		modified by this function
 * @return	Zero if the operation succeeds, otherwise an error code
 */
void lane_grayscale_apply(lane_image_t *image);

#endif /* LANE_GRAYSCALE_H */
