/**
 * @file lane_image.c
 * @author Matthijs Bakker
 * @brief Generic image utilities
 *
 * This code unit provides various utilities for working
 * with images, including the creation of images and the
 * modification of them.
 */

#include "lane_image.h"

#include <math.h>
#include <string.h>

#include "lane_log.h"

/*
 * @inheritDoc
 */
lane_image_t *lane_image_new(uint16_t width, uint16_t height) {
	lane_image_t *result = malloc(sizeof(lane_image_t));

	// The data memory is not cleared upon allocation because
	// we want to fill it immediately afterwards in most cases
	result->width = width;
	result->height = height;
	result->data = malloc(width * (height+1) * sizeof(lane_pixel_t));

	return result;
}

/*
 * @inheritDoc
 */
lane_image_t *lane_image_copy(const lane_image_t *const image) {
	lane_image_t *result = lane_image_new(image->width, image->height);

	memcpy(result->data, image->data, image->width * image->height * sizeof(lane_pixel_t));
	
	return result;
}

/*
 * @inheritDoc
 */
void lane_image_fill_solid(lane_image_t *image, lane_pixel_t color) {
	int x, y;

	// Imagine using memcpy....
	// we're doing it the manly way!
	for (y = 0; y < image->height; ++y) {
		for (x = 0; x < image->width; ++x) {
			image->data[(y * image->width) + x] = color;
		}
	}
}

/*
 * @inheritDoc
 */
void lane_image_draw_line(lane_image_t *image, lane_pixel_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	int dx, dy, sx, sy, err, e2;

	// Check if the line is within bounds of the image
	if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0
		|| x1 > image->width || x2 > image->width
		|| y1 > image->height || y2 > image->height) {

		LANE_LOG_ERROR("Line from (%d,%d) to (%d,%d) is out of bounds (%d,%d), aborting", x1, y1, x2, y2, image->width, image->height);

		return; // TODO return error code?
	}

	dx = abs(x2 - x1);
	sx = x1 < x2 ? 1 : -1;
	dy = abs(y2 - y1);
	sy = y1 < y2 ? 1 : -1;
	err = (dx > dy ? dx : -dy) / 2;
	e2 = 0;

	// Use Bresenham's algorithm for drawing a line
	while (1) {
		// If the destination point is reached
		// Stop before drawing the destination because
		// otherwise valgrind complains, for some reason.
		if (x1 == x2 && y1 == y2) {
			break;
		}

		image->data[(y1 * image->width) + x1] = color;

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

/*
 * @inheritDoc
 */
void lane_image_free(lane_image_t *image) {
	free(image->data);
	free(image);
}

