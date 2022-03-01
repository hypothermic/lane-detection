#include <stdio.h>
#include <stdlib.h>

#include "lane_gaussian.h"
#include "lane_grayscale.h"
#include "lane_hough.h"
#include "lane_image.h"
#include "lane_image_ppm.h"
#include "lane_log.h"
#include "lane_kmeans.h"
#include "lane_sobel.h"
#include "lane_threshold.h"

/**
 * @see test/lane_hough_overlay_test.c#HOUGH_THRESHOLD
 */
#define HOUGH_THRESHOLD		(150)

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
	FILE *input_file = NULL,
	     *output_file = NULL;
	lane_image_t *input = NULL,
		     *overlay = NULL;
	lane_hough_normal_t *normals = NULL;
	lane_hough_space_t *space = NULL;
	lane_kmeans_medoid_t *medoids = NULL;
	size_t lines_amount, i;

	if (argc < 3) {
		LANE_LOG_ERROR("Argument 1 must be the filename of the PPM image and argument 2 must be a destination");
		return 1;
	}

	input_file = fopen(argv[1], "rb");

	if (!input_file) {
		LANE_LOG_ERROR("File '%s' cannot be opened", argv[1]);
		return 2;
	}

	if (lane_image_ppm_from_file(input_file, &input)) {
		LANE_LOG_ERROR("Error while loading image from file '%s'", argv[1]);
		return 3;
	}

	if (input_file) {
		fclose(input_file);
	}
	
	lines_amount = lane_hough_apply(input, &space, &normals, HOUGH_ANGLE_MIN, HOUGH_ANGLE_MAX, HOUGH_THRESHOLD);
	lane_kmeans_apply(normals, lines_amount, &medoids, KMEANS_ITERATIONS, KMEANS_CLUSTERS);

	// plot lines onto copy of current image to create a nice overlay
	overlay = lane_image_copy(input);
	for (i = 0; i < KMEANS_CLUSTERS; ++i) {
		lane_kmeans_medoid_plot(overlay, space, medoids[i]);
	}

	output_file = fopen(argv[2], "wb");

	if (!output_file) {
		LANE_LOG_ERROR("Output file '%s' cannot be opened", argv[2]);
	}

	if (lane_image_ppm_to_file(output_file, overlay)) {
		LANE_LOG_ERROR("Error while outputting to file '%s'", argv[2]);
		return 4;
	}

	if (output_file) {
		fclose(output_file);
	}

	lane_image_free(input);
	lane_image_free(overlay);
	free(normals);
	free(medoids);
	free(space->acc);
	free(space);

	return 0;
}
