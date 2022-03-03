/**
 * @file lane_hough.c
 * @author Matthijs Bakker
 * @brief Utilities for working with the Hough Transform
 *
 * This code unit provides various utilities for working
 * with the Hough Transform, including a function to generate
 * the Hough Space for an image, and tools for extracting/
 * plotting lines resulting from this space.
 */

#include "lane_hough.h"

#include <math.h>

#include "lane_log.h"

/**
 * @internal
 *
 * Converts degrees to radians.
 */
#define RADIANS(degrees)					(degrees * M_PI / 180L)

/**
 * @internal
 *
 * Normalizes a normal line (rho, theta) to a Cartesian line
 */
#define NORMALIZE(x, y, cx, cy, th, h)				(round((((double) x - cx) * cos(RADIANS(th)) + (((double) y - cy) * sin(RADIANS(th)))) + h))

/**
 * @internal
 *
 * Converts a Cartesian line to a polar line
 */
#define POLARIZE(coord, relx, rely, line, space, o1, o2)	((double) (line.rho - ((int) space->height / 2)) - ((coord - (relx / 2)) * o1(RADIANS(line.theta)))) / o2(RADIANS(line.theta)) + (rely / 2)

/**
 * @internal
 *
 * Checks if a line is vertical (between 45 and 135 degrees)
 */
#define IS_LINE_VERTICAL(line)					(line.theta >= 45 && line.theta <= 135)

/**
 * @internal
 */
#define WHITE_THRESHOLD						(128)

/**
 * @internal
 */
#define IS_WHITE(pixel)						(pixel.r > WHITE_THRESHOLD)

/**
 * @internal
 */
#define HEIGHT_FACTOR						(2L)

/**
 * @internal
 */
#define KERNEL_SIZE						(4)

/**
 * @internal
 */
#define INITIAL_ARRAY_SIZE					(50)

/**
 * @internal
 *
 * Fills an accumulator from the image using the Hough voting technique.
 *
 * @param image		The target image, which will not be modified
 * @param space		The accumulator where votes will be written to
 * @param h		The height of the accumulator
 * @param min		The minimum value of theta to compute rho for
 * @param max		The maximum value of theta to compute rho for
 */
static inline void quantize(const lane_image_t *const image, lane_hough_space_t *space, double h, uint8_t min, uint8_t max);

/**
 * @internal
 *
 * Extracts lines from the Hough Space/accumulator
 *
 * @param lines		Where the resulting lines will be stored
 * @param space		The accumulator to extract from
 * @param thres		The threshold to apply when extracting lines
 *
 * @return		The amount of lines that were classified
 */
static inline size_t classify(lane_hough_normal_t **lines, const lane_hough_space_t *const space, uint16_t thres);

/**
 * @internal
 *
 * Checks if the magnitude of a cell in the accumulator is higher than
 * that of the neighboring cells. (i.e. detects a peak in the accumulator)
 *
 * @param space		The accumulator where the cells are stored
 * @param rho		The rho value of the cell
 * @param th		The theta value of the cell
 * 
 * @return		The magnitude of the target cell
 */
static inline uint32_t magnitude(const lane_hough_space_t *const space, uint16_t rho, uint16_t th);

/*
 * @inheritDoc
 */
size_t lane_hough_apply(const lane_image_t *const src, lane_hough_space_t **rspace, lane_hough_normal_t **rnormals, uint8_t min, uint8_t max, uint16_t thres) {
	lane_hough_space_t *space;
	lane_hough_normal_t *lines;
	double height, rads;
	size_t lines_amount;//, x, y;
	
	rads = max - min;
	height = (sqrt(HEIGHT_FACTOR) * (double)(src->height>src->width?src->height:src->width)) / HEIGHT_FACTOR;
	
	space = malloc(sizeof(lane_hough_space_t));
	space->width = rads;
	space->height = height * HEIGHT_FACTOR;
	space->size = space->width * space->height;
	space->acc = calloc(space->size, sizeof(uint32_t));
	
	if (!space->acc) {
		LANE_LOG_ERROR("Allocating of accumulator failed; aborting");

		return 0;
	}

	quantize(src, space, height, min, max);
	lines_amount = classify(&lines, space, thres);

	(*rspace) = space;
	(*rnormals) = lines;

	return lines_amount;
}

#define DegreesToRadians(deg)	RADIANS(deg)

/*
 * @inheritDoc
 */
lane_hough_resolved_line_t lane_hough_resolve_line(lane_image_t *image, const lane_hough_space_t *const space, const lane_hough_normal_t line) {
	lane_hough_resolved_line_t result;
	int x1, y1, x2, y2;

	// Check from which corner we need to base the line
	if (IS_LINE_VERTICAL(line)) {
		x1 = 0;
		y1 = POLARIZE(x1, image->width, image->height, line, space, cos, sin);
		x2 = image->width;
		y2 = POLARIZE(x2, image->width, image->height, line, space, cos, sin);
	} else {
		y1 = 0;
		x1 = POLARIZE(y1, image->height, image->width, line, space, sin, cos);
		y2 = image->height;
		x2 = POLARIZE(y2, image->height, image->width, line, space, sin, cos);
	}

	result.x1 = x1;
	result.y1 = y1;
	result.x2 = x2;
	result.y2 = y2;

	LANE_LOG_INFO("Resolved line with rho=%04d, th=%03d, estimating from (%04d, %04d) to (%04d, %04d)", line.rho, line.theta, result.x1, result.y1, result.x2, result.y2);

	return result;
}

/*
 * @inheritDoc
 */
void lane_hough_plot_line(lane_image_t *image, const lane_hough_resolved_line_t *const line) {
	LANE_LOG_INFO("Reported line %d, %d, %d, %d", line->x1, line->y1, line->x2, line->y2);
	lane_image_draw_line(image, (lane_pixel_t) {255, 0, 0}, line->x1, line->y1, line->x2, line->y2);
}

/*
 * @inheritDoc
 */
void lane_hough_plot_graph(lane_image_t *image, const lane_hough_space_t *const space) {
	lane_pixel_t pixel;
	uint32_t x, y, val;

	// Most values from HT are within 0-300 so it's not worth
	// to correct them on a scale
	// Peaks will be fully white on grayscale anyway

	// Copy pixel-by-pixel and cutoff the value to fit it
	// within the 8-bit color channel of the output
	for (y = 0; y < space->height; ++y) {
		for (x = 0; x < space->width; ++x) {
			val = space->acc[(y * space->width) + x];

			if (val > 255) { // uint8_t max
				val = 255;
			}

			pixel.r = val;
			pixel.g = val;
			pixel.b = val;

			image->data[(y * space->width) + x] = pixel;
		}
	}
}

/*
 * @inheritDoc
 */
static inline void quantize(const lane_image_t *const image, lane_hough_space_t *space, double h, uint8_t min, uint8_t max) {
	uint16_t x, y;
	uint8_t th;
	double cx, cy;

	// Center coordinates of the image
	cx = image->width / 2;
	cy = image->height / 2;

	// Create a Hough Space by quantizing the input
	for (y = 0; y < image->height; ++y) {
		for (x = 0; x < image->width; ++x) {
			if (IS_WHITE(image->data[(y*image->width) + x])) {
				for (th = min; th < max; ++th) {
					space->acc[(int) (th + (space->width * NORMALIZE(x, y, cx, cy, th, h)))]++;
				}
			}
		}
	}
}

/*
 * @inheritDoc
 */
static inline uint32_t magnitude(const lane_hough_space_t *const space, uint16_t rho, uint16_t th) {
	uint32_t magnitude;
	uint16_t x, y;

	// Magnitude of the target pixel
	magnitude = space->acc[(rho * space->width) + th];

	// Get the magnitude of neighboring pixels
	for (y = -KERNEL_SIZE; y <= KERNEL_SIZE; ++y) {
		for (x = -KERNEL_SIZE; x <= KERNEL_SIZE; ++x) {
			if ((y + rho >= 0 && y + rho < space->height) && (x + th >= 0 && x + th < space->width)) {
				if ((int) space->acc[((rho + y) * space->width) + (th + x)] > (int) magnitude) {
					magnitude = space->acc[((rho + y) * space->width) + (th + x)];
					goto found;
				}
			}
		}
	}

found:	return magnitude;
}

/*
 * @inheritDoc
 */
static inline size_t classify(lane_hough_normal_t **lines, const lane_hough_space_t *const space, uint16_t thres) {
	lane_hough_normal_t *results;
	size_t amount, alloc;
	uint16_t rho, th;

	// I use calloc here because the struct members will be "initialized"
	// and Valgrind won't complain anymore...
	amount = 0;
	alloc = INITIAL_ARRAY_SIZE;
	results = calloc(alloc, sizeof(lane_hough_normal_t));

	if (!results) {
		LANE_LOG_ERROR("Unable to allocate memory for lines; aborting");
		return 0;
	}

	// Loop over accumulator to find peaks
	// And make sure this pixel is higher than all neighbor pixels
	for (rho = 0; rho < space->height; ++rho) {
		for (th = 0; th < space->width; ++th) {
			if ((int) space->acc[(rho * space->width) + th] >= thres) {
				if (magnitude(space, rho, th) <= space->acc[(rho * space->width) + th]) {
					
					// Peak detected!
					LANE_LOG_INFO("Detected rho=%04d th=%04d", rho, th);

					if (amount >= (alloc - 1)) {
						alloc *= 2;
						results = realloc(results, alloc * sizeof(lane_hough_normal_t));
						
						if (!results) {
							LANE_LOG_ERROR("Unable to realloc memory for lines; aborting");

							return 0;
						}
					}

					results[amount++] = (lane_hough_normal_t) {
						.rho=rho,
						.theta=th
					};
				}
			}
		}
	}
	
	(*lines) = results;

	return amount;
}

