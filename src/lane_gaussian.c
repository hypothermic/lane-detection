#include "lane_gaussian.h"

#include "math.h"

/**
 * @inheritDoc
 */
void lane_gaussian_apply(const lane_image_t *const src, lane_image_t **dest, uint8_t size, double variance) {
	lane_image_t *out;
	lane_pixel_t next;
	double kernel[size][size], total, r, g, b;
	int x, y, i, j, index, radius, ir;

	// Assuming that 'size' is an odd number,
	// the radius of the kernel is ((size-1)/2)
	radius = (size - 1) / 2;
	// The inclusive radius is the radius plus the center point
	ir = radius + 1;

	out = lane_image_new(src->width, src->height);

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
	for (y = ir; y < src->height - (ir * 2); ++y) {
		for (x = ir; x < src->width - (ir * 2); ++x) {
			r = g = b = 0.0;
			index = (y * src->width) + x;

			// Convolve the kernel with the image around the
			// current pixel to calculate the RGB value for x,y
			for (i = 0; i < size; ++i) {
				for (j = 0; j < size; ++j) {
					r += src->data[(y - radius + i) * src->width + x - radius + j].r * kernel[i][j];
					g += src->data[(y - radius + i) * src->width + x - radius + j].g * kernel[i][j];
					b += src->data[(y - radius + i) * src->width + x - radius + j].b * kernel[i][j];
				}
			}

			// Round the values to the nearest integer value
			// and put them in the output image
			next.r = r;
			next.g = g;
			next.b = b;
			out->data[index] = next;
		}
	}

	(*dest) = out;
}

