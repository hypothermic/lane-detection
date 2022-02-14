#include "lane_image.h"

/**
 * @inheritDoc
 */
lane_image_t *lane_image_new(uint16_t width, uint16_t height) {
	lane_image_t *result = malloc(sizeof(lane_image_t));

	result->width = width;
	result->height = height;
	result->data = malloc(width * height * sizeof(lane_pixel_t));

	return result;
}

/**
 * @inheritDoc
 */
void lane_image_fill_solid(lane_image_t *image, lane_pixel_t color) {
	int x, y;

	for (y = 0; y < image->height; ++y) {
		for (x = 0; x < image->width; ++x) {
			image->data[(y * image->width) + x] = color;
		}
	}
}

/**
 * @inheritDoc
 */
void lane_image_free(lane_image_t *image) {
	free(image->data);
	free(image);
}

