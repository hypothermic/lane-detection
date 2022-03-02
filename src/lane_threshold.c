/**
 * @file lane_threshold.c
 * @author Matthijs Bakker
 * @brief Image thresholding for grayscale images
 *
 * This code unit provides a function to threshold monotone
 * images using upper and lower boundaries.
 */

#include "lane_grayscale.h"

/*
 * @inheritDoc
 */
void lane_threshold_apply(lane_image_t *image, uint8_t lower, uint8_t upper, uint8_t new) {
	lane_pixel_t pixel;
	size_t x, y, index;

	// Loop over each pixel
	for (y = 0; y < image->height; ++y) {
		for (x = 0; x < image->width; ++x) {
			index = (y * image->width) + x;
			pixel = image->data[index];

			// If the pixel value falls within the threshold,
			// don't modify it.
			if (pixel.r >= lower && pixel.r <= upper &&
			    pixel.g >= lower && pixel.g <= upper &&
			    pixel.b >= lower && pixel.b <= upper) {
				continue;
			}

			// The value if not within the threshold.
			// Replace it with the new value.
			image->data[index].r = new;
			image->data[index].g = new;
			image->data[index].b = new;
		}
	}
}

