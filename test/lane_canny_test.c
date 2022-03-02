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
 * The level of thresholding applied after the Sobel filter
 */
#define ARTIFACT_THRESHOLD	(80)

int main(int argc, char **argv) {
	lane_image_t *input = NULL,
		     *sobel = NULL,
		     *edges = NULL;
	double *directions = NULL;

	TEST_CHECK_ARGS(argc, argv);

	TEST_LOAD_IMAGE(argv[1], input);

	LANE_PROFILE(sobel, lane_sobel_apply(input, &sobel, &directions));
	LANE_PROFILE(nonmax, lane_nonmax_apply(sobel, directions, &edges))

	TEST_SAVE_IMAGE(argv[2], edges);

	lane_image_free(input);
	lane_image_free(sobel);
	lane_image_free(edges);
	free(directions);

	return 0;
}

