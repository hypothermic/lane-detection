#include <stdio.h>
#include <stdlib.h>

#include "lane_image.h"
#include "lane_image_ppm.h"

#define LANE_LOG_ENABLE
#include "lane_log.h"

int main(int argc, char **argv) {
	lane_image_t *image = NULL;
	FILE *file = NULL;

	if (argc < 2) {
		LANE_LOG_ERROR("Argument 1 must be the filename of the PPM image");
		return 1;
	}

	file = fopen(argv[1], "rb");

	if (!file) {
		LANE_LOG_ERROR("File '%s' cannot be opened", argv[1]);
		return 2;
	}

	if (lane_image_ppm_from_file(file, &image)) {
		LANE_LOG_ERROR("Error while loading image from file '%s'", argv[1]);
		return 3;
	}

	if (file) {
		fclose(file);
	}

	return 0;
}


