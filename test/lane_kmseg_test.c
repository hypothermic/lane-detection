#include <stdio.h>
#include <stdlib.h>

#include "lane_grayscale.h"
#include "lane_gaussian.h"
#include "lane_image.h"
#include "lane_image_ppm.h"
#include "lane_kmeans.h"
#include "lane_sobel.h"
#include "lane_threshold.h"
#include "lane_laplace.h"
#include "lane_log.h"

#define ITERATIONS	10
#define CLUSTERS	2

int main(int argc, char **argv) {
	FILE *input_file = NULL,
	     *output_file = NULL;
	lane_image_t *image = NULL,
		     *blurred = NULL,
		     *edges = NULL,
		     *overlay = NULL;
	lane_hough_resolved_line_t *lines = NULL;
	lane_hough_normal_t *normals = NULL;
	lane_hough_space_t *space = NULL;
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

	if (lane_image_ppm_from_file(input_file, &image)) {
		LANE_LOG_ERROR("Error while loading image from file '%s'", argv[1]);
		return 3;
	}

	if (input_file) {
		fclose(input_file);
	}
	
	LANE_PROFILE(grayscale, lane_grayscale_apply(image));
	LANE_PROFILE(kmseg, lane_kmeans_segment(image, ITERATIONS, CLUSTERS));
	LANE_PROFILE(gaussian, lane_gaussian_apply(image, &blurred, 5, 2));
	LANE_PROFILE(sobel, lane_sobel_apply(blurred, &edges));
	LANE_PROFILE(threshold, lane_threshold_apply(edges, 210, 255, 0));
	LANE_PROFILE(hough, lines_amount = lane_hough_apply(edges, &space, &normals, 0, 180, 150));

	LANE_PROFILE(overlay, {
		overlay = lane_image_copy(edges);
		lines = calloc(lines_amount, sizeof(lane_hough_resolved_line_t));
		for (i = 0; i < lines_amount; ++i) {
			lines[i] = lane_hough_resolve_line(overlay, space, normals[i]);
			lane_hough_plot_line(overlay, &(lines[i]));
		}
	})

	LANE_LOG_INFO("%lu lines were plotted", lines_amount);

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

	lane_image_free(image);
	lane_image_free(blurred);
	lane_image_free(edges);
	lane_image_free(overlay);
	free(lines);
	free(normals);
	free(space->acc);
	free(space);

	return 0;
}


