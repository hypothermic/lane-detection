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
 * @see test/lane_hough_overlay_test.c#HOUGH_THRESHOLD
 */
#define HOUGH_THRESHOLD		(100)

/**
 * @see test/lane_hough_overlay_test.c#HOUGH_ANGLE_MIN
 */
#define HOUGH_ANGLE_MIN		(0)

/**
 * @see test/lane_hough_overlay_test.c#HOUGH_ANGLE_MAX
 */
#define HOUGH_ANGLE_MAX		(180)

/**
 * Amount of clusters to use
 */
#define KMEANS_CLUSTERS		(2)

/**
 * Amount of iterations to run
 */
#define KMEANS_ITERATIONS	(255)

int main(int argc, char **argv) {
	lane_image_t *input = NULL,
		     *overlay = NULL;
	lane_hough_normal_t *normals = NULL;
	lane_hough_space_t *space = NULL;
	lane_kmeans_medoid_t *medoids = NULL;
	size_t lines_amount, i;

	TEST_CHECK_ARGS(argc, argv);

	TEST_LOAD_IMAGE(argv[1], input);

	lines_amount = lane_hough_apply(input, &space, &normals, HOUGH_ANGLE_MIN, HOUGH_ANGLE_MAX, HOUGH_THRESHOLD);
	lane_kmeans_apply(normals, lines_amount, &medoids, KMEANS_ITERATIONS, KMEANS_CLUSTERS);

	// plot lines onto copy of current image to create a nice overlay
	overlay = lane_image_copy(input);
	for (i = 0; i < KMEANS_CLUSTERS; ++i) {
		lane_kmeans_medoid_plot(overlay, space, medoids[i]);
	}

	TEST_SAVE_IMAGE(argv[2], overlay);

	lane_image_free(input);
	lane_image_free(overlay);
	free(normals);
	free(medoids);
	free(space->acc);
	free(space);

	return 0;
}

