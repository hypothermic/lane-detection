#ifndef LANE_LOG_H
#define LANE_LOG_H

/**
 * A switch to toggle the inclusion of log statements in the
 * program code. Can be controlled via the option in the Makefile.
 */
#ifdef LANE_LOG_ENABLE

/**
 * Prints an information message to the standard output.
 */
#define LANE_LOG_INFO(format, ...)	printf("INFO | " format "\n", ##__VA_ARGS__)

/**
 * Prints an error message to the standard error stream and flushes it.
 */
#define LANE_LOG_ERROR(format, ...)	fprintf(stderr, "ERROR | " format "\n", ##__VA_ARGS__)

#else

#define LANE_LOG_INFO(format, ...)	({(void)0;})
#define LANE_LOG_ERROR(format, ...)	({(void)0;})

#endif /* LANE_LOG_ENABLE */

#endif /* LANE_LOG_H */
