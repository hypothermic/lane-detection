#ifndef LANE_LAPLACE_H
#define LANE_LAPLACE_H

#include "lane_image.h"

/**
 *
 * Apply an iteration of Laplacian to an image.<br />
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
void lane_laplace_apply(const lane_image_t *const src, lane_image_t **dest);

#endif /* LANE_LAPLACE_H */
