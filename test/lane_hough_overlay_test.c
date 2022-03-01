#include <stdio.h>
#include <stdlib.h>

#include "lane_gaussian.h"
#include "lane_grayscale.h"
#include "lane_hough.h"
#include "lane_image.h"
#include "lane_image_ppm.h"
#include "lane_kmeans.h"
#include "lane_log.h"
#include "lane_sobel.h"
#include "lane_test_common.h"
#include "lane_threshold.h"

/**
 * Accumulator value threshold for HT
 */
#define HOUGH_THRESHOLD		(150)

/**
 * Minimum angle of the range.
 */
#define HOUGH_ANGLE_MIN		(0)

/**
 * Maximum angle of the range.
 */
#define HOUGH_ANGLE_MAX		(180)

int main(int argc, char **argv) {
	lane_image_t *input = NULL,
		     *overlay = NULL;
	lane_hough_resolved_line_t *lines = NULL;
	lane_hough_normal_t *normals = NULL;
	lane_hough_space_t *space = NULL;
	size_t lines_amount, i;

	TEST_CHECK_ARGS(argc, argv);

	TEST_LOAD_IMAGE(argv[1], input);
	
	lines_amount = lane_hough_apply(input, &space, &normals, HOUGH_ANGLE_MIN, HOUGH_ANGLE_MAX, HOUGH_THRESHOLD);

	// plot lines onto copy of current image to create a nice overlay
	overlay = lane_image_copy(input);
	lines = calloc(lines_amount, sizeof(lane_hough_resolved_line_t));
	for (i = 0; i < lines_amount; ++i) {
		lines[i] = lane_hough_resolve_line(overlay, space, normals[i]);
		lane_hough_plot_line(overlay, &(lines[i]));
	}

	LANE_LOG_INFO("%lu lines were plotted", lines_amount);

	TEST_SAVE_IMAGE(argv[2], overlay);

	lane_image_free(input);
	lane_image_free(overlay);
	free(lines);
	free(normals);
	free(space->acc);
	free(space);

	return 0;
}

