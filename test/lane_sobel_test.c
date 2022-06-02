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
#define ARTIFACT_THRESHOLD	(100) // ~80%... (256/100)*80=205

int main(int argc, char **argv) {
	lane_image_t *input = NULL,
		     *output = NULL;
	double *directions = NULL;
	TEST_CHECK_ARGS(argc, argv);

	TEST_LOAD_IMAGE(argv[1], input);

	LANE_PROFILE(sobel, lane_sobel_apply(input, &output, &directions));
	LANE_PROFILE(threshold, lane_threshold_apply(output, ARTIFACT_THRESHOLD, 255, 0, false));

	TEST_SAVE_IMAGE(argv[2], output);

	lane_image_free(input);
	lane_image_free(output);
	free(directions);

	return 0;
}

