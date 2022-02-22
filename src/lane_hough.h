#ifndef LANE_HOUGH_H
#define LANE_HOUGH_H

#include "lane_image.h"

/**
 * @inheritDoc
 */
typedef struct normal		lane_hough_normal_t;

/**
 * @inheritDoc
 */
typedef struct space		lane_hough_space_t;

/**
 * @inheritDoc
 */
typedef struct resolved_line	lane_hough_resolved_line_t;

/**
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
 * The Hough accumulator space.
 */
struct space {
	uint32_t *acc, width, height, size;
};

/**
 * A resolved line in Cartesian space.
 */
struct resolved_line {
	uint16_t x1, y1, x2, y2;
};

/**
 * Isolate lines within an image by using Classical Hough Transform.<br />
 * <br />
 * This function returns a visual representation of the accumulator
 * and a overlap image with line marks.
 *
 * @param src		The input image, which data will be read
 * @param acc		A visual representation of the accumulator
 * @param ov		An overlayed image with detected lines
 * @param rlines	Output for the resolved lines array
 * @param min		Minimum value of theta to compute rho for
 * @param max		Maximum value of theta to compute rho for
 * @param thres		Threshold for accumulator values
 * @return		Zero or higher, indicating the amount of
 * 			lines that were detected
 */
size_t lane_hough_apply(const lane_image_t *const src, lane_image_t **acc, lane_image_t **ov, lane_hough_resolved_line_t **rlines, uint8_t min, uint8_t max, uint16_t thres);

#endif /* LANE_HOUGH_H */
