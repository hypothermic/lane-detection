#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lane_gaussian.h"
#include "lane_grayscale.h"
#include "lane_image.h"
#include "lane_image_ppm.h"
#include "lane_log.h"
#include "lane_sobel.h"
#include "lane_test_common.h"
#include "lane_threshold.h"

/**
 * The threshold at which edges are questionable
 */
#define LOWER_THRESHOLD		(8)

/**
 * The threshold at which edges are certain
 */
#define UPPER_THRESHOLD		(64)

int main(int argc, char **argv) {
	lane_image_t *input = NULL,
		     *sobel = NULL,
		     *edges = NULL,
		     *weak = NULL,
		     *strong = NULL;
	double *directions = NULL;

	TEST_CHECK_ARGS(argc, argv);

	TEST_LOAD_IMAGE(argv[1], input);

	LANE_PROFILE(sobel, lane_sobel_apply(input, &sobel, &directions));
	LANE_PROFILE(nonmax, lane_nonmax_apply(sobel, directions, &edges));

	weak = lane_image_copy(edges);
	strong = lane_image_copy(edges);

	lane_threshold_apply(weak, LOWER_THRESHOLD, (UPPER_THRESHOLD - 1), 64, true);
	lane_threshold_apply(strong, UPPER_THRESHOLD, 255, 255, true);
	lane_image_add(weak, strong);
	lane_hysteresis_apply(weak, 64, 255);

	TEST_SAVE_IMAGE(argv[2], weak);

	lane_image_free(input);
	lane_image_free(sobel);
	lane_image_free(edges);
	lane_image_free(weak);
	lane_image_free(strong);
	free(directions);

	return 0;
}

