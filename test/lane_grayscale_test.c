#include <stdio.h>
#include <stdlib.h>

#include "lane_grayscale.h"
#include "lane_image.h"
#include "lane_image_ppm.h"
#include "lane_log.h"
#include "lane_test_common.h"

int main(int argc, char **argv) {
	lane_image_t *image = NULL;

	TEST_CHECK_ARGS(argc, argv);

	TEST_LOAD_IMAGE(argv[1], image);
	
	LANE_PROFILE(grayscale, lane_grayscale_apply(image));

	TEST_SAVE_IMAGE(argv[2], image);
	
	lane_image_free(image);

	return 0;
}


