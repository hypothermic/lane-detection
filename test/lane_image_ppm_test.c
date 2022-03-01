#include <stdio.h>
#include <stdlib.h>

#include "lane_image.h"
#include "lane_image_ppm.h"
#include "lane_log.h"
#include "lane_test_common.h"

// To verify the PPM load/save functionality,
// this test just loads the image into memory
// and then saves it to a file

int main(int argc, char **argv) {
	lane_image_t *image = NULL;

	TEST_CHECK_ARGS(argc, argv);

	TEST_LOAD_IMAGE(argv[1], image);

	TEST_SAVE_IMAGE(argv[2], image);

	lane_image_free(image);

	return 0;
}

