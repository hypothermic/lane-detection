#ifndef LANE_LOG_H
#define LANE_LOG_H

#include <time.h>

/**
 * A switch to toggle the inclusion of log statements in the
 * program code. Can be controlled via the option in the Makefile.
 */
#ifdef LANE_LOG_ENABLE

/**
 * Prints an information message to the standard output.
 */
#define LANE_LOG_INFO(format, ...)	\
	printf(TERM_ESCAPE TERM_COLOR_INFO "INFO | " \
		TERM_ESCAPE TERM_COLOR_FOREGROUND format \
		TERM_ESCAPE TERM_RESET "\n", ##__VA_ARGS__)

/**
 * Prints an error message to the standard error stream and flushes it.
 */
#define LANE_LOG_ERROR(format, ...)	\
	fprintf(stderr, TERM_ESCAPE TERM_COLOR_WARNING "ERROR | " \
		TERM_ESCAPE TERM_COLOR_FOREGROUND format \
		TERM_ESCAPE TERM_RESET "\n", ##__VA_ARGS__)

#define LANE_PROFILING_START(section)	\
	struct timespec __start_##section , __end_##section ; \
	clock_gettime(CLOCK_REALTIME, &__start_##section ); \
	LANE_LOG_INFO("Entering section " #section);

#define LANE_PROFILING_END(section)	\
	clock_gettime(CLOCK_REALTIME, &__end_##section ); \
	LANE_LOG_INFO("Completed section \"" #section "\" in %lf sec", \
			( __end_##section .tv_nsec - __start_##section .tv_nsec ) / 1e9 + \
			( __end_##section .tv_sec - __start_##section .tv_sec ));

#define LANE_PROFILE(section, stmt)	\
	LANE_PROFILING_START(section)	\
	stmt;				\
	LANE_PROFILING_END(section)

#define TERM_ESCAPE			"\x1b["
#define TERM_COLOR_INFO			"30;2m"
#define TERM_COLOR_WARNING		"31m"
#define TERM_COLOR_FOREGROUND		"m"
#define TERM_RESET			"0m"

#else

#define LANE_LOG_INFO(format, ...)	({(void)0;})
#define LANE_LOG_ERROR(format, ...)	({(void)0;})

#endif /* LANE_LOG_ENABLE */

#endif /* LANE_LOG_H */
