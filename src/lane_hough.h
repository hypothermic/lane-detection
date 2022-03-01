/**
 * @file lane_hough.h
 * @author Matthijs Bakker
 * @brief Utilities for working with the Hough Transform
 *
 * This code unit provides various utilities for working
 * with the Hough Transform, including a function to generate
 * the Hough Space for an image, and tools for extracting/
 * plotting lines resulting from this space.
 */

#ifndef LANE_HOUGH_H
#define LANE_HOUGH_H

#include "lane_image.h"

/**
 * @copydoc normal
 */
typedef struct normal		lane_hough_normal_t;

/**
 * @copydoc space
 */
typedef struct space		lane_hough_space_t;

/**
 * @copydoc resolved_line
 */
typedef struct resolved_line	lane_hough_resolved_line_t;

/**
 * @brief A line represented by rho, theta values
 *
 * A normal line which is represented by:<br />
 * <br />
 * rho = x * cos(th) + y * sin(th)<br />
 * <br />
 * where rho is the distance from the origin and
 * th is the angle between the line and the x axis.
 */
struct normal {
	uint16_t rho, theta;
};

/**
 * @brief Accumulator for Hough transform
 *
 * The Hough accumulator space where votes are casted.<br />
 * <br />
 * The size is dependent on the input image size.
 */
struct space {
	uint32_t *acc, width, height, size;
};

/**
 * @brief A line represented in Cartesian space
 *
 * A resolved line in Cartesian space.<br />
 * <br />
 * This includes the start and end coordinates.
 */
struct resolved_line {
	uint16_t x1, y1, x2, y2;
};

/**
 * @brief Use the Hough Transform to isolate lines
 *
 * Isolate lines within an image by using Classical Hough Transform.<br />
 * <br />
 * This function returns the accumulator and the resulting lines.
 *
 * @param src		The input image, which data will be read
 * @param space		The resulting accumulator / Hough space
 * @param rnormals	Output for the normals array
 * @param min		Minimum value of theta to compute rho for
 * @param max		Maximum value of theta to compute rho for
 * @param thres		Threshold for accumulator values
 * @return		Zero or higher, indicating the amount of
 * 			lines that were detected
 */
size_t lane_hough_apply(const lane_image_t *const src, lane_hough_space_t **space, lane_hough_normal_t **rnormals, uint8_t min, uint8_t max, uint16_t thres);

/**
 * @brief Resolve a line from polar coordinates to Cartesian coordinates
 *
 * Resolve a line from it's r,t value within Hough parameter space
 * to its coordinates in Cartesian space.<br />
 * <br />
 * Note that the start and end values will also be tried to match.
 * 
 * @param image		The image with the sizes of the Cartesian space
 * @param space		The Hough space
 * @param line		The line to resolve
 * @return		A resolved line structure with the {start,end}-{x,y}
 */
lane_hough_resolved_line_t lane_hough_resolve_line(lane_image_t *image, const lane_hough_space_t *const space, const lane_hough_normal_t line);

/**
 * Draw a line on an image
 *
 * @param image		The image to draw the line on
 * @param line		The line to draw
 */
void lane_hough_plot_line(lane_image_t *image, const lane_hough_resolved_line_t *const line);

/**
 * Draw the accumulator values on an image
 *
 * @param image		The image to draw the accumulator values on
 * @param space		The accumulator to draw
 */
void lane_hough_plot_graph(lane_image_t *image, const lane_hough_space_t *const space);

#endif /* LANE_HOUGH_H */
