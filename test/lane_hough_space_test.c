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
		     *visualization = NULL;
	lane_hough_normal_t *normals = NULL;
	lane_hough_space_t *space = NULL;

	TEST_CHECK_ARGS(argc, argv);

	TEST_LOAD_IMAGE(argv[1], input);

	(void) lane_hough_apply(input, &space, &normals, HOUGH_ANGLE_MIN, HOUGH_ANGLE_MAX, HOUGH_THRESHOLD);

	// output the accumulator to the image
	visualization = lane_image_new(space->width, space->height);
	lane_hough_plot_graph(visualization, space);

	TEST_SAVE_IMAGE(argv[2], visualization);

	lane_image_free(input);
	lane_image_free(visualization);
	free(normals);
	free(space->acc);
	free(space);

	return 0;
}

