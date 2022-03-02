/**
 * @file lane_image.h
 * @author Matthijs Bakker
 * @brief Generic image utilities
 *
 * This code unit provides various utilities for working
 * with images, including the creation of images and the
 * modification of them.
 */

#ifndef LANE_IMAGE_H
#define LANE_IMAGE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * The value of a color channel at a specific location
 */
typedef uint8_t		lane_color_t;

/**
 * @copydoc pixel
 */
typedef struct pixel	lane_pixel_t;

/**
 * @copydoc image
 */
typedef struct image	lane_image_t;

/**
 * @brief A pixel within an image
 *
 * A dot on the screen with a mixture of red, green, and blue colors.<br />
 * <br />
 * For grayscale images, r=g=b. But we still use all those channels.
 */
struct pixel {
	lane_color_t r, g, b;
};

/**
 * @brief An image in memory
 *
 * A representation of an image.<br />
 * <br />
 * The data is horizontally stored in a 2d array.
 */
struct image {
	uint16_t width, height;
	lane_pixel_t *data;
};

/**
 * Allocates a blank new image.
 *
 * @param width		The width in pixels of the new image
 * @param height	The height in pixels of the new image
 * @return		A pointer to the struct
 */
lane_image_t *lane_image_new(uint16_t width, uint16_t height);

/**
 * Allocates a new image, copying the data from another.
 *
 * @param image		Image to be copied
 * @return		A pointer to the newly allocated image
 */
lane_image_t *lane_image_copy(const lane_image_t *const image);

/**
 * @brief Fill an image with a color
 *
 * Fill an image with a solid color.<br />
 * <br />
 * This is useful for testing if an image has been cropped correctly.
 *
 * @param image		The image that will be altered
 * @param color		The color that will be set for each pixel
 */
void lane_image_fill_solid(lane_image_t *image, lane_pixel_t color);

/**
 * Draw a simple line on an image using Bresenham.
 *
 * @param image		The image that will be altered
 * @param color		The color that will be set for each pixel
 * @param x1		The X coordinate of the first point
 * @param y1		The Y coordinate of the first point
 * @param x2		The X coordinate of the second point
 * @param y2		The Y coordinate of the second point
 */
void lane_image_draw_line(lane_image_t *image, lane_pixel_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**
 * Merge two images by adding the pixel values together.
 *
 * @param image		The source image that will be added upon
 * @param additions	The other image that whose values will be used
 */
void lane_image_add(lane_image_t *image, const lane_image_t *const additions);

/**
 * Deallocates an image and its associated data.
 *
 * @param image		The image to be deallocated
 */
void lane_image_free(lane_image_t *image);

#endif /* LANE_IMAGE_H */

