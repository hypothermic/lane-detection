#ifndef LANE_GAUSSIAN_H
#define LANE_GAUSSIAN_H

#include "lane_image.h"

/**
 * Blur an image by applying the Gaussian function on it.<br />
 * <br />
 * <b>Note:</b> The output image will be cropped by <i>(size-1)*2</i> px
 * because there is no data to apply the kernel on in the borders of
 * the image!
 *
 * @param src		The input image, which data will be read
 * @param dest		The output image, which will be (over)written to
 * @param size		The size of the kernel
 * @param variance	The sigma value of the Gaussian function
 */
void lane_gaussian_apply(const lane_image_t *const src, lane_image_t **dest, uint8_t size, double variance);

#endif /* LANE_GAUSSIAN_H */
