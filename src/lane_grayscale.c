/**
 * @file lane_grayscale.c
 * @author Matthijs Bakker
 * @brief RGB to grayscale filter for images
 *
 * This code unit provides a filter for converting images
 * from the RGB colorspace to grayscale using the BT.601-7 weights.
 */

#include "lane_grayscale.h"

// MATLAB rgb2gray uses the weights from Rec.ITU-R BT.601-7:
// 0.2989 * R + 0.5870 * G + 0.1140 * B
// source: [https://nl.mathworks.com/help/matlab/ref/rgb2gray.html]

/**
 * @internal
 */
#define CHANNEL_R_WEIGHT	0.2989

/**
 * @internal
 */
#define CHANNEL_G_WEIGHT	0.5870

/**
 * @internal
 */
#define CHANNEL_B_WEIGHT	0.1140

/*
 * @inheritDoc
 */
void lane_grayscale_apply(lane_image_t *image) {
	size_t x, y, index, value;

	// MATLAB rgb2gray uses the following constants:
	// 0.2989 * R + 0.5870 * G + 0.1140 * B
	// so we will use them as well

	for (y = 0; y < image->height; ++y) {
		for (x = 0; x < image->width; ++x) {
			index = (y * image->width) + x;

			value	= image->data[index].r * CHANNEL_R_WEIGHT
				+ image->data[index].g * CHANNEL_G_WEIGHT
				+ image->data[index].b * CHANNEL_B_WEIGHT;

			image->data[index].r = value;
			image->data[index].g = value;
			image->data[index].b = value;
		}
	}
}

