#include <stdio.h>
#include <stdlib.h>

#include "lane_gaussian.h"
#include "lane_grayscale.h"
#include "lane_image.h"
#include "lane_image_ppm.h"
#include "lane_laplace.h"
#include "lane_log.h"
#include "lane_test_common.h"
#include "lane_threshold.h"

int main(int argc, char **argv) {
	lane_image_t *input = NULL,
		     *output = NULL;

	TEST_CHECK_ARGS(argc, argv);
	
	TEST_LOAD_IMAGE(argv[1], input);

	LANE_PROFILE(laplace, lane_laplace_apply(input, &output));

	TEST_SAVE_IMAGE(argv[2], output);

	lane_image_free(input);
	lane_image_free(output);

	return 0;
}

