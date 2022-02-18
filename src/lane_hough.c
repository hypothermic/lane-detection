#include "lane_hough.h"

#include <math.h>

#include "lane_log.h"

#define RADIANS(degrees)	(degrees * M_PI / 180L)
#define IS_WHITE(pixel)		(pixel.r > 128)
#define HEIGHT_FACTOR		(2.0L)

static inline void quantize(const lane_image_t *const image, lane_hough_space_t *space, double h, uint8_t min, uint8_t max);

static inline size_t classify(lane_hough_normal_t **lines, const lane_hough_space_t *const space, int thres);

static inline void plot_line(lane_image_t *image, const lane_hough_space_t *const space, const lane_hough_normal_t line);

static inline void plot_graph(lane_image_t *image, const lane_hough_space_t *const space);

/**
 * @inheritDoc
 */
void lane_hough_apply(const lane_image_t *const src, lane_image_t **acc, lane_image_t **ov, uint8_t min, uint8_t max, uint16_t thres) {
	lane_image_t *graph, *overlay;
	lane_hough_space_t space;
	lane_hough_normal_t *lines;
	double height, rads;
	size_t lines_amount, x, y;
	
	rads = max - min;
	height = ((sqrt(HEIGHT_FACTOR) * (double)(src->height>src->width?src->height:src->width)) / HEIGHT_FACTOR);
	
	space.width = rads;
	space.height = height * HEIGHT_FACTOR;
	space.size = space.width * space.height;
	space.acc = calloc(space.size, sizeof(uint32_t));

	graph = lane_image_new(space.width, space.height);
	overlay = lane_image_new(src->width, src->height);
	
	if (!space.acc) {
		LANE_LOG_ERROR("Allocating of accumulator failed; aborting");

		return; // TODO return error code?
	}


	quantize(src, &space, height, min, max);

	lines_amount = classify(&lines, &space, thres);

	for (y = 0; y < src->height; ++y) {
		for (x = 0; x < src->width; ++x) {
			overlay->data[(y * src->width) + x] = src->data[(y * src->width) + x];
		}
	}

	do {
		plot_line(overlay, &space, lines[lines_amount]);
	} while (--lines_amount);


	plot_graph(graph, &space);

	free(lines);
	free(space.acc);

	(*acc) = graph;
	(*ov) = overlay;
}

static inline void quantize(const lane_image_t *const image, lane_hough_space_t *space, double h, uint8_t min, uint8_t max) {
	double cx, cy, r;

	cx = image->width / 2;
	cy = image->height / 2;
	r = 0;

	for (int y = 0; y < image->height; ++y) {
		for (int x = 0; x < image->width; ++x) {
			if (IS_WHITE(image->data[(y*image->width) + x])) {
				for (int th = min; th < max; ++th) {
					r = (((double) x - cx) * cos((double) RADIANS(th))) + (((double) y - cy) * sin((double) RADIANS(th)));
					space->acc[(int)((round(r + h) * space->width)) + th]++;
				}
			}
		}
	}
}

static inline size_t classify(lane_hough_normal_t **lines, const lane_hough_space_t *const space, int thres) {
	lane_hough_normal_t *results;
	size_t amount;
	
	amount = 0;
	results = malloc(200 * sizeof(lane_hough_normal_t)); // TODO change size
	
	if (!results) {
		LANE_LOG_ERROR("Unable to allocate memory for lines; aborting");
		return 0;
	}

	for (int rho = 0; rho < space->height; ++rho) {
		for (int th = 0; th < space->width; ++th) {
			if ((int) space->acc[(rho * space->width) + th] >= thres) {
				int lm = space->acc[(rho * space->width) + th];
				
				for (int ly = -4; ly <= 4; ++ly) {
					for (int lx = -4; lx <= 4; ++lx) {
						if ((ly + rho >= 0 && ly + rho < space->height) && (lx + th >= 0 && lx + th < space->width)) {
							if ((int) space->acc[((rho + ly) * space->width) + (th + lx)] > lm) {
								lm = space->acc[((rho + ly) * space->width) + (th + lx)];
								ly = lx = 5;
							}
						}
					}
				}

				if (lm > (int) space->acc[(rho * space->width) + th]) {
					continue;
				}

				// detected!

				LANE_LOG_INFO("Detected rho=%04d th=%04d", rho, th);
				results[amount++] = (lane_hough_normal_t) {
					.rho=(uint16_t)rho,
					.theta=(uint16_t)th
				};
			}
		}
	}
	
	(*lines) = results;

	return amount;
}

static inline void plot_line(lane_image_t *image, const lane_hough_space_t *const space, const lane_hough_normal_t line) {
	int x1, y1, x2, y2;

	if (line.theta >= 45 && line.theta <= 135) {
		x1 = 0;
		y1 = ((double) (line.rho - ((int) space->height / 2)) - ((x1 - (image->width / 2)) * cos(RADIANS(line.theta)))) / sin(RADIANS(line.theta)) + (image->height / 2);
		x2 = image->width - 0;
		y2 = ((double) (line.rho - ((int) space->height / 2)) - ((x2 - (image->width / 2)) * cos(RADIANS(line.theta)))) / sin(RADIANS(line.theta)) + (image->height / 2);
	} else {
		y1 = 0;
		x1 = ((double) (line.rho - ((int) space->height / 2)) - ((y1 - (image->height / 2)) * sin(RADIANS(line.theta)))) / cos(RADIANS(line.theta)) + (image->width / 2); 
		y2 = image->height - 0;
		x2 = ((double) (line.rho - ((int) space->height / 2)) - ((y2 - (image->height / 2)) * sin(RADIANS(line.theta)))) / cos(RADIANS(line.theta)) + (image->width / 2);
	}

	LANE_LOG_INFO("Line with rho=%04d, th=%03d, estimating from (%04d, %04d) to (%04d, %04d)", line.rho, line.theta, x1, y1, x2, y2);

	lane_image_draw_line(image, (lane_pixel_t){255, 0, 0}, x1, y1, x2, y2);
}

static inline void plot_graph(lane_image_t *image, const lane_hough_space_t *const space) {
	lane_pixel_t pixel;
	int x, y, val;

	// Most values from HT are within 0-300 so it's not worth
	// to correct them on a scale

	// Copy pixel-by-pixel and cutoff the value to fit it
	// within the 8-bit color channel of the output
	for (y = 0; y < space->height; ++y) {
		for (x = 0; x < space->width; ++x) {
			val = (int) space->acc[(y * space->width) + x];

			if (val < 0) {
				val = 0;
			}

			if (val > 255) {
				val = 255;
			}

			pixel.r = val;
			pixel.g = val;
			pixel.b = val;

			image->data[(y * space->width) + x] = pixel;
		}
	}
}

