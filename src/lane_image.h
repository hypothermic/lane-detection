#ifndef LANE_IMAGE_H
#define LANE_IMAGE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint8_t		lane_color_t;
typedef struct pixel	lane_pixel_t;
typedef struct image	lane_image_t;

struct pixel {
	lane_color_t r, g, b;
};

struct image {
	uint16_t width, height;
	lane_pixel_t *data;
};

/**
 * Allocates a blank new image.
 */
lane_image_t *lane_image_new(uint16_t width, uint16_t height);

#endif /* LANE_IMAGE_H */

