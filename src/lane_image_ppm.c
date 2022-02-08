#include "lane_image_ppm.h"

#include <inttypes.h>

#define LANE_LOG_ENABLE
#include "lane_log.h"


/**
 * The total length of the magic string,
 * which should be "P6\n"
 */
#define LINE_BUFFER_SIZE	1024

/**
 * Macro for reading a line into a buffer.
 */
#define READ_LINE		\
	if (!fgets(line_buffer, LINE_BUFFER_SIZE, file)) { \
		return 2; \
	}

/**
 * Prevent loading big images into memory by setting a limit
 *
 * Without setting a limit, because of the data types, the
 * maximum would be around 65535²; approx 4 GB
 */
#define MAX_IMAGE_DIMENSIONS	4096

/**
 * @inheritDoc
 */
int lane_image_ppm_from_file(FILE *file, lane_image_t **image) {
	lane_image_t *out;
	char line_buffer[LINE_BUFFER_SIZE];
	uint16_t width, height;
	uint8_t *raw;
	int result, acc;

	if (!file) {
		LANE_LOG_ERROR("No file specified");
		return 1;
	}

	// Read the first line with the magic code (should be 'PX\n')
	// where X is the PPM version number.

	READ_LINE;

	LANE_LOG_INFO("Reading PPM with format %s", line_buffer);

	// P3 image format is ASCII-padded
	// P6 is in binary, which is what we want
	if (line_buffer[1] != '6') {
		return 3;
	}
	
	// Skip comments
	while (line_buffer[0] == '#' || line_buffer[0] == 'P') {
		READ_LINE;
	}

	// Read image dimensions
	result = sscanf(line_buffer, "%u %u", &width, &height);	
	if (result < 2) {
		LANE_LOG_ERROR("Error while reading image dimensions");
		return 5;
	}
	
	out = lane_image_new(width, height);
	(*image) = out;	
	
	LANE_LOG_INFO("Input image is %u x %u", width, height);

	if (width > MAX_IMAGE_DIMENSIONS || height > MAX_IMAGE_DIMENSIONS) {
		LANE_LOG_ERROR("Image (%lu x %lu px) is larger than allowed (%lu x %3$lu px)",
				width, height, MAX_IMAGE_DIMENSIONS);
		return 6;
	}

	READ_LINE; // skip bpp value
	//fseek(file, 1, SEEK_CUR); // skip newline symbol

	// read from file into raw
	raw = malloc(width * height * sizeof(lane_pixel_t));
	result = fread(raw, sizeof(lane_pixel_t), width * height, file);
	if (result < width * height) {
		LANE_LOG_ERROR("Expected %lu bytes, only %lu bytes read", (width * height), result);
		return 7;
	}

	// organize raw data into struct;
	acc = 0;
	for (size_t i = 0; i < (width * height); ++i) {
		out->data[i].r = raw[acc++];
		out->data[i].g = raw[acc++];
		out->data[i].b = raw[acc++];
	}

	LANE_LOG_INFO("Image data loaded into struct");
	
	free(raw);

	return 0;
}

/**
 * @inheritDoc
 */
int lane_image_ppm_to_file(FILE *file, lane_image_t *image) {
	size_t acc = 0;
	uint8_t write_buffer[3];
	lane_pixel_t current_pixel;
	
	// Check if file stream is valid
	if (!file) {
		LANE_LOG_ERROR("No file specified");
		return 1;
	}

	// write magic and header
	fprintf(file, "P6\n%d %d\n255\n", image->width, image->height);

	// Loop horizontally over the image, 
	for (size_t col = 0; col < image->height; ++col) {
		for (size_t row = 0; row < image->width; ++row) {

			// Write all three color bytes at once
			// by first loading them into a buffer
			current_pixel = image->data[acc++];

			write_buffer[0] = current_pixel.r;
			write_buffer[1] = current_pixel.g;
			write_buffer[2] = current_pixel.b;

			fwrite(write_buffer, 1, 3, file);
		}
	}

	return 0;
}

