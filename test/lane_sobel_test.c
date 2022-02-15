#include <stdio.h>
#include <stdlib.h>

#include "lane_gaussian.h"
#include "lane_grayscale.h"
#include "lane_image.h"
#include "lane_image_ppm.h"
#include "lane_log.h"
#include "lane_sobel.h"
#include "lane_threshold.h"

/**
 * @see test/lane_gaussian_test.c#GAUSSIAN_SIZE
 */
#define GAUSSIAN_SIZE		(5)

/**
 * @see test/lane_gaussian_test.c#GAUSSIAN_VARIANCE
 */
#define GAUSSIAN_VARIANCE	(4.0)

/**
 * The level of thresholding applied after the Sobel filter
 */
#define ARTIFACT_THRESHOLD	(250)

int main(int argc, char **argv) {
	FILE *input_file = NULL,
	     *output_file = NULL;
	lane_image_t *input = NULL,
		     *blurred = NULL,
		     *output = NULL;

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

	// 1. convert to grayscale
	// 2. apply gaussian blur
	// 3. apply sobel filter
	// 4. only keep pixels which are fully white

	lane_grayscale_apply(input);
	lane_gaussian_apply(input, &blurred, GAUSSIAN_SIZE, GAUSSIAN_VARIANCE);
	lane_sobel_apply(input, &output);
	lane_threshold_apply(output, ARTIFACT_THRESHOLD, 255, 0);

	output_file = fopen(argv[2], "wb");

	if (!output_file) {
		LANE_LOG_ERROR("Output file '%s' cannot be opened", argv[2]);
	}

	if (lane_image_ppm_to_file(output_file, output)) {
		LANE_LOG_ERROR("Error while outputting to file '%s'", argv[2]);
		return 4;
	}

	if (output_file) {
		fclose(output_file);
	}

	return 0;
}

