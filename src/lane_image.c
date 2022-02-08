#include "lane_image.h"

lane_image_t *lane_image_new(uint16_t width, uint16_t height) {
	lane_image_t *result = malloc(sizeof(lane_image_t));

	result->width = width;
	result->height = height;
	result->data = malloc(width * height * sizeof(lane_pixel_t));

	return result;
}
