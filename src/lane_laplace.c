/**
 * @file lane_laplace.c
 * @author Matthijs Bakker
 * @brief Laplacian filter for images
 *
 * This code unit provides a Laplacian filter which convolutes
 * a predefined 3 by 3 Laplacian kernel with an image. This
 * has the effect of highlighting gradients/edges in the image.
 */

#include "lane_laplace.h"

#include <math.h>

/**
 * @internal
 *
 * The diameter of the kernel
 */
#define KERNEL_DIAMETER	3 //

/**
 * @internal
 *
 * The radius of the kernel
 */
#define KERNEL_RADIUS	1 // result of (3-1)/2

/**
 * @internal
 *
 * The Laplacian kernel which gets convoluted with the image
 */
const int kernel[KERNEL_DIAMETER][KERNEL_DIAMETER] = {
	{ 0, -1,  0},
	{-1,  4, -1},
	{ 0, -1,  0}
};

/*
 * @inheritDoc
 */
void lane_laplace_apply(const lane_image_t *const src, lane_image_t **dest) {
	lane_image_t *out;
	int x, y, m, i, j, si, ci;

	// Because the kernel cannot be convoluted with the 1 pixel
	// border of the input image (there are no neighbor pixels)
	// we have to cut off a part of the image and correct for it.
	out = lane_image_new(src->width - KERNEL_RADIUS * 2, src->height - KERNEL_RADIUS * 2);

	// For each pixel
	for (y = KERNEL_RADIUS; y < src->height - KERNEL_RADIUS; ++y) {
		for (x = KERNEL_RADIUS; x < src->width - KERNEL_RADIUS; ++x) {
			m = 0;
			
			// Convolve the kernel with the original image
			// on neighbor pixels to get the pixel magnitude
			for (i = 0; i < KERNEL_DIAMETER; ++i) {
				for (j = 0; j < KERNEL_DIAMETER; ++j) {
					si = (y - KERNEL_RADIUS + i) * src->width + x - KERNEL_RADIUS + j;
					m += src->data[si].r * kernel[i][j];
				}
			}

			// Restrict the gradient magnitude within bounds
			if (m < 0) m = 0;
			if (m > 255) m = 255;
	
			// Calculate the corrected index of the target image
			// (this takes into consideration the cropping)
			ci = ((y - KERNEL_RADIUS) * out->width) + (x - KERNEL_RADIUS);

			// Update the output pixel
        		out->data[ci].r = m;
        		out->data[ci].g = m;
        		out->data[ci].b = m;
		}
	}

	(*dest) = out;
}

