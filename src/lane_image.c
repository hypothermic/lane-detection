#include "lane_image.h"

#include <math.h>

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
void lane_image_draw_line(lane_image_t *image, lane_pixel_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	int dx = abs(x2 - x1),
	    sx = x1 < x2 ? 1 : -1,
	    dy = abs(y2 - y1),
	    sy = y1 < y2 ? 1 : -1,
	    err = (dx > dy ? dx : -dy) / 2,
	    e2 = 0;

	while (1) {
		image->data[(y1 * image->width)] = color;
		
		if (x1 == x2 && y1 == y2) {
			break;
		}

		e2 = err;
		
		if (e2 > -dx) {
			err -= dy;
			x1 += sx;
		}
		
		if (e2 < dy) {
			err += dx;
			y1 += sy;
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

