/**
 * @file lane_test_common.h
 * @author Matthijs Bakker
 * @brief Commonly used routines for tests
 *
 * This header file provides macros for routines that are
 * commonly used for testing. They are defined in this
 * shared header file to save space and clean up code in
 * the individual test files.
 */

#ifndef LANE_TEST_COMMON_H
#define LANE_TEST_COMMON_H

#define TEST_CHECK_ARGS(argc, argv) \
	if (argc < 3) { \
		LANE_LOG_ERROR("Argument 1 must be the filename of the PPM image and argument 2 must be a destination"); \
		return 1; \
	}

#define TEST_LOAD_IMAGE(file, dest) \
	FILE *input_file = NULL; \
	input_file = fopen(file, "rb"); \
	if (!input_file) { \
		LANE_LOG_ERROR("File '%s' cannot be opened", file); \
		return 2; \
	} \
	if (lane_image_ppm_from_file(input_file, &dest)) { \
		LANE_LOG_ERROR("Error while loading image from file '%s'", file); \
		return 3; \
	} \
	if (input_file) { \
		fclose(input_file); \
	}

#define TEST_SAVE_IMAGE(file, src) \
	FILE *output_file = NULL; \
	output_file = fopen(file, "wb"); \
	if (!output_file) { \
		LANE_LOG_ERROR("Output file '%s' cannot be opened", file); \
	} \
	if (lane_image_ppm_to_file(output_file, src)) { \
		LANE_LOG_ERROR("Error while outputting to file '%s'", file); \
		return 4; \
	} \
	if (output_file) { \
		fclose(output_file); \
	}

#endif /* LANE_TEST_COMMON_H */
