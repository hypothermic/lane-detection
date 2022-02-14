#ifndef LANE_THRESHOLD_H
#define LANE_THRESHOLD_H

#include "lane_image.h"

#define LANE_THRESHOLD_UNUSED_LOWER	0
#define LANE_THRESHOLD_UNUSED_UPPER	255

/**
 * Discard pixels with a value outside of a specific bound
 *
 * @param image	The image that will be modified by this function
 * @param lower	The inclusive lower bound of the threshold (0 for unused)
 * @param upper	The inclusive upper bound of the threshold (255 for unused)
 * @param new	The replacement value when outside of threshold
 *
 * @return	Zero if the operation succeeds, otherwise an error code
 */
void lane_threshold_apply(lane_image_t *image, uint8_t lower, uint8_t upper, uint8_t new);

#endif /* LANE_THRESHOLD_H */
