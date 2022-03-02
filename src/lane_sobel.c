/**
 * @file lane_sobel.c
 * @author Matthijs Bakker
 * @brief Sobel filter for images
 *
 * This code unit provides a Sobel filter which convolutes
 * two predefined 3x3 kernels with an image and calculates
 * the gradient magnitude for each pixel.
 */

#include "lane_sobel.h"

#include <math.h>

/**
 * @internal
 *
 * The diameter of a kernel
 */
#define KERNEL_DIAMETER	3 // predefined Sobel kernel diameter

/**
 * @internal
 *
 * The radius of a kernel
 */
#define KERNEL_RADIUS	1 // result of (3-1)/2

/**
 * @internal
 *
 * Horizontal kernel of the Sobel operator
 */
const int kx[KERNEL_DIAMETER][KERNEL_DIAMETER] = {
	{-1,  0,  1},
	{-2,  0,  2},
	{-1,  0,  1}
};

/**
 * @internal
 *
 * Vertical kernel of the Sobel operator
 */
const int ky[KERNEL_DIAMETER][KERNEL_DIAMETER] = {
	{ 1,  2,  1},
	{ 0,  0,  0},
	{-1, -2, -1},
};

/*
 * @inheritDoc
 */
void lane_sobel_apply(const lane_image_t *const src, lane_image_t **dest) {
	lane_image_t *out;
	int x, y, mx, my, m, i, j, si, ci;

	// Because the kernel cannot be convoluted with the 1 pixel
	// border of the input image (there are no neighbor pixels)
	// we have to cut off a part of the image and correct for it.
	out = lane_image_new(src->width - KERNEL_RADIUS * 2, src->height - KERNEL_RADIUS * 2);

	// For each pixel
	for (y = KERNEL_RADIUS; y < src->height - KERNEL_RADIUS; ++y) {
		for (x = KERNEL_RADIUS; x < src->width - KERNEL_RADIUS; ++x) {
			mx = my = 0;
			
			// Convolve kernels with the original image to get
			// the horizontal and vertical magnitudes (mx, my)
			for (i = 0; i < KERNEL_DIAMETER; ++i) {
				for (j = 0; j < KERNEL_DIAMETER; ++j) {
					si = (y - KERNEL_RADIUS + i) * src->width + x - KERNEL_RADIUS + j;
					mx += src->data[si].r * kx[i][j];
					my += src->data[si].r * ky[i][j];
				}
			}

			// Calculate the gradient magnitude
			m = sqrt(pow(mx, 2) + pow(my, 2));

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

