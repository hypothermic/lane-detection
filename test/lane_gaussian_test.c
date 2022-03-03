#include <stdio.h>
#include <stdlib.h>

#include "lane_gaussian.h"
#include "lane_grayscale.h"
#include "lane_image.h"
#include "lane_image_ppm.h"
#include "lane_log.h"
#include "lane_test_common.h"

/**
 * The size of the Gaussian kernel.
 */
#define GAUSSIAN_SIZE		(5)

/**
 * The sigma term; the impact of the
 * neighboring pixels on the target pixel.<br />
 * <br />
 * Increasing this will make the image seem more blurred.
 */
#define GAUSSIAN_VARIANCE	(2.5)

int main(int argc, char **argv) {
	lane_image_t *image = NULL,
		     *out = NULL;
	
	TEST_CHECK_ARGS(argc, argv);

	TEST_LOAD_IMAGE(argv[1], image);
	
	LANE_PROFILE(gaussian, lane_gaussian_apply(image, &out, GAUSSIAN_SIZE, GAUSSIAN_VARIANCE));

	TEST_SAVE_IMAGE(argv[2], out);

	lane_image_free(image);
	lane_image_free(out);

	return 0;
}


