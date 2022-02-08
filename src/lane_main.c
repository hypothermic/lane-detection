#include <stdio.h>
#include <stdlib.h>

#include "lane_image.h"
#include "lane_image_ppm.h"
#include "lane_log.h"

int main(int argc, char **argv) {
	lane_image_t *image = NULL;
	FILE *file, *outfile = NULL;

	if (argc < 3) {
		LANE_LOG_ERROR("Argument 1 must be the filename of the PPM image and argument 2 must be a destination");
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

	outfile = fopen(argv[2], "wb");

	if (!outfile) {
		LANE_LOG_ERROR("Output file '%s' cannot be opened", argv[2]);
	}

	if (lane_image_ppm_to_file(outfile, image)) {
		LANE_LOG_ERROR("Error while outputting to file '%s'", argv[2]);
		return 4;
	}

	if (outfile) {
		fclose(outfile);
	}

	return 0;
}


