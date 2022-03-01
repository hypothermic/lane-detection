/**
 * @file lane_image_ppm.h
 * @author Matthijs Bakker
 * @brief Load/save images in PPM format
 *
 * This code unit provides the loading and saving of
 * images in the Portable Pixmap format.<br />
 * <br />
 * It only supports the P6 (binary colored images) standard.
 */

#ifndef LANE_IMAGE_PPM_H
#define LANE_IMAGE_PPM_H

#include "lane_image.h"

/**
 * @brief Load an image from a PPM file
 *
 * Allocates a new image and fills it with data from a PPM file.
 *
 * @param file	File in PPM format to read the image from
 * @param image	The destination for the image
 *
 * @return	Zero if the operation succeeds, otherwise an error code
 */
int lane_image_ppm_from_file(FILE *file, lane_image_t **image);

/**
 * @brief Write an image to a PPM file
 *
 * Writes an image to a file in PPM format.
 * If the destination file exists, it will be overwritten.
 *
 * @param file	File destination
 * @param image	The image that needs to be written
 *
 * @return	Zero if the operation succeeds, otherwise an error code
 */
int lane_image_ppm_to_file(FILE *file, lane_image_t *image);

#endif /* LANE_IMAGE_PPM_H */

