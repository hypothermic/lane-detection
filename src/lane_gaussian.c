/**
 * @file lane_gaussian.c
 * @author Matthijs Bakker
 * @brief Gaussian blur filter for images
 *
 * This code unit provides a filter for applying Gaussian blur to images.
 */

#include "lane_gaussian.h"

#include <math.h>

/*
 * @inheritDoc
 */
void lane_gaussian_apply(const lane_image_t *const src, lane_image_t **dest, uint8_t size, double variance) {
	lane_image_t *out;
	lane_pixel_t next;
	double kernel[size][size], total, r, g, b;
	int x, y, i, j, si, ci, radius, ir;

	// Assuming that 'size' is an odd number,
	// the radius of the kernel is ((size-1)/2)
	radius = (size - 1) / 2;
	// The inclusive radius is the radius plus the center point
	ir = radius + 1;
	// Reset the total value
	total = 0;

	// It's kinda confusing because we have to crop the image.
	// The borders of the source image won't be used if there are
	// no pixels for the kernel te be applied to. So we always
	// end up with a smaller output image than input image.
	// Variable si is the source index and ci is the corrected index
	out = lane_image_new(src->width - (2 * ir), src->height - (2 * ir));

	// Calculate the values for the kernel assuming its size
	for (y = 0; y < size; ++y) {
		for (x = 0; x < size; ++x) {
			kernel[y][x] = exp(((pow((y - radius), 2) + pow((x - radius), 2)) / ((2 * pow(variance, 2)))) * (-1));
			total += kernel[y][x];
		}
	}

	// Create a factor between 0...1 for the values
	for (y = 0; y < size; ++y) {
		for (x = 0; x < size; ++x) {
			kernel[y][x] /= total;
		}
	}

	// Iterate over each pixel in a horizontal manner
	for (y = ir; y < src->height - ir; ++y) {
		for (x = ir; x < src->width - ir; ++x) {
			r = g = b = 0.0;
			// The source index (si) corresponds to the pixel
			// in the src image and the corrected index (ci)
			// is the relative index in the cropped output image
			// This was a nightmare to debug.......
			ci = ((y - ir) * out->width) + (x - ir);

			// Convolve the kernel with the image around the
			// current pixel to calculate the RGB value for x,y
			for (i = 0; i < size; ++i) {
				for (j = 0; j < size; ++j) {
					si = (y - radius + i) * src->width + x - radius + j;
					r += src->data[si].r * kernel[i][j];
					g += src->data[si].g * kernel[i][j];
					b += src->data[si].b * kernel[i][j];
				}
			}

			// Round the values to the nearest integer value
			// and put them in the output image
			next.r = r;
			next.g = g;
			next.b = b;

			out->data[ci] = next;
		}
	}

	(*dest) = out;
}

