#include "lane_hough.h"

#include <math.h>

#include "lane_log.h"

#define RADIANS(degrees)					(degrees * M_PI / 180L)
#define NORMALIZE(x, y, cx, cy, th, h)				(round((((double) x - cx) * cos(RADIANS(th)) + (((double) y - cy) * sin(RADIANS(th)))) + h))
#define POLARIZE(coord, relx, rely, line, space, o1, o2)	((double) (line.rho - ((int) space->height / 2)) - ((coord - (relx / 2)) * o1(RADIANS(line.theta)))) / o2(RADIANS(line.theta)) + (rely / 2)
#define IS_LINE_HORIZONTAL(line)				(line.theta >= 45 && line.theta <= 135)

#define WHITE_THRESHOLD						(128)
#define IS_WHITE(pixel)						(pixel.r > WHITE_THRESHOLD)

#define HEIGHT_FACTOR						(2L)
#define KERNEL_SIZE						(4)
#define INITIAL_ARRAY_SIZE					(50)

static inline void quantize(const lane_image_t *const image, lane_hough_space_t *space, double h, uint8_t min, uint8_t max);

static inline size_t classify(lane_hough_normal_t **lines, const lane_hough_space_t *const space, uint16_t thres);

static inline uint32_t magnitude(const lane_hough_space_t *const space, uint16_t rho, uint16_t th);

static inline lane_hough_resolved_line_t resolve_line(lane_image_t *image, const lane_hough_space_t *const space, const lane_hough_normal_t line);

static inline void plot_line(lane_image_t *image, const lane_hough_resolved_line_t *const line);

static inline void plot_graph(lane_image_t *image, const lane_hough_space_t *const space);

/**
 * @inheritDoc
 */
size_t lane_hough_apply(const lane_image_t *const src, lane_image_t **acc, lane_image_t **ov, lane_hough_resolved_line_t **rlines, uint8_t min, uint8_t max, uint16_t thres) {
	lane_image_t *graph, *overlay;
	lane_hough_space_t space;
	lane_hough_normal_t *lines;
	lane_hough_resolved_line_t *resolved;
	double height, rads;
	size_t lines_amount, i;//, x, y;
	
	rads = max - min;
	height = (sqrt(HEIGHT_FACTOR) * (double)(src->height>src->width?src->height:src->width)) / HEIGHT_FACTOR;
	
	space.width = rads;
	space.height = height * HEIGHT_FACTOR;
	space.size = space.width * space.height;
	space.acc = calloc(space.size, sizeof(uint32_t));

	graph = lane_image_new(space.width, space.height);
	overlay = lane_image_copy(src);
	
	if (!space.acc) {
		LANE_LOG_ERROR("Allocating of accumulator failed; aborting");

		return 0;
	}

	quantize(src, &space, height, min, max);
	lines_amount = classify(&lines, &space, thres);

	resolved = calloc(lines_amount, sizeof(lane_hough_resolved_line_t));
	for (i = 0; i < lines_amount; ++i) {
		resolved[i] = resolve_line(overlay, &space, lines[i]);
		plot_line(overlay, &(resolved[i]));
	}

	plot_graph(graph, &space);

	free(lines);
	free(space.acc);

	(*acc) = graph;
	(*ov) = overlay;
	(*rlines) = resolved;

	return lines_amount;
}

static inline void quantize(const lane_image_t *const image, lane_hough_space_t *space, double h, uint8_t min, uint8_t max) {
	uint16_t x, y, th;
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

static inline uint32_t magnitude(const lane_hough_space_t *const space, uint16_t rho, uint16_t th) {
	uint32_t magnitude;
	uint16_t x, y;

	// Magnitude of the target pixel
	magnitude = space->acc[(rho * space->width) + th];

	// Get the magnitude of neighboring pixels
	for (y = -KERNEL_SIZE; y <= KERNEL_SIZE; ++y) {
		for (x = -KERNEL_SIZE; x <= KERNEL_SIZE; ++x) {
			if ((y + rho >= 0 && y + rho < space->height) && (x + th >= 0 && x + th < space->width)) {
				if ((int) space->acc[((rho + y) * space->width) + (th + x)] > magnitude) {
					magnitude = space->acc[((rho + y) * space->width) + (th + x)];
					goto found;
				}
			}
		}
	}

found:	return magnitude;
}

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
				if (magnitude(space, rho, th) <= (int) space->acc[(rho * space->width) + th]) {
					
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

static inline lane_hough_resolved_line_t resolve_line(lane_image_t *image, const lane_hough_space_t *const space, const lane_hough_normal_t line) {
	lane_hough_resolved_line_t result;
	uint16_t x1, y1, x2, y2;

	// Check from which corner we need to base the line
	if (IS_LINE_HORIZONTAL(line)) {
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

	LANE_LOG_INFO("Resolved line with rho=%04d, th=%03d, estimating from (%04d, %04d) to (%04d, %04d)", line.rho, line.theta, x1, y1, x2, y2);

	return result;
}

static inline void plot_line(lane_image_t *image, const lane_hough_resolved_line_t *const line) {
	lane_image_draw_line(image, (lane_pixel_t) {255, 0, 0}, line->x1, line->y1, line->x2, line->y2);
}

static inline void plot_graph(lane_image_t *image, const lane_hough_space_t *const space) {
	lane_pixel_t pixel;
	int x, y, val;

	// Most values from HT are within 0-300 so it's not worth
	// to correct them on a scale
	// Peaks will be fully white on grayscale anyway

	// Copy pixel-by-pixel and cutoff the value to fit it
	// within the 8-bit color channel of the output
	for (y = 0; y < space->height; ++y) {
		for (x = 0; x < space->width; ++x) {
			val = (int) space->acc[(y * space->width) + x];

			if (val < 0) { // uint8_t min
				val = 0;
			}

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

