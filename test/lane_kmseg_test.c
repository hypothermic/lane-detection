#include <stdio.h>
#include <stdlib.h>

#include "lane_grayscale.h"
#include "lane_gaussian.h"
#include "lane_image.h"
#include "lane_image_ppm.h"
#include "lane_kmeans.h"
#include "lane_laplace.h"
#include "lane_log.h"
#include "lane_sobel.h"
#include "lane_test_common.h"
#include "lane_threshold.h"

/**
 * How many iterations to run
 */
#define KMEANS_ITERATIONS	(10)

/**
 * How many clusters to form (2 = lane / non-lane)
 */
#define KMEANS_CLUSTERS		(2)

int main(int argc, char **argv) {
	lane_image_t *input = NULL;

	TEST_CHECK_ARGS(argc, argv);

	TEST_LOAD_IMAGE(argv[1], input);
	
	LANE_PROFILE(kmseg, lane_kmeans_segment(input, KMEANS_ITERATIONS, KMEANS_CLUSTERS));

	TEST_SAVE_IMAGE(argv[2], input);

	lane_image_free(input);

	return 0;
}


