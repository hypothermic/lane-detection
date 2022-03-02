/**
 * @file lane_log.h
 * @author Matthijs Bakker
 * @brief Logging and profiling utilities
 *
 * This header file provides various logging macros which
 * log to the standard output and the standard error output.<br />
 * <br />
 * It also provides profiling macros which insert profiling
 * code at the beginning and end of a user-defined section.
 */

#ifndef LANE_LOG_H
#define LANE_LOG_H

#include <time.h>

/**
 * @brief Toggle logging on/off
 *
 * A switch to toggle the inclusion of log statements in the
 * program code. Can be controlled via the option in the Makefile.
 */
#ifdef LANE_LOG_ENABLE

/**
 * @brief Log an info message to stdout
 *
 * Prints an information message to the standard output.
 */
#define LANE_LOG_INFO(format, ...)	\
	printf(TERM_ESCAPE TERM_COLOR_INFO "INFO | " \
		TERM_ESCAPE TERM_COLOR_FOREGROUND format \
		TERM_ESCAPE TERM_RESET "\n", ##__VA_ARGS__)

/**
 * @brief log an error message to stderr
 *
 * Prints an error message to the standard error stream and flushes it.
 */
#define LANE_LOG_ERROR(format, ...)	\
	fprintf(stderr, TERM_ESCAPE TERM_COLOR_WARNING "ERROR | " \
		TERM_ESCAPE TERM_COLOR_FOREGROUND format \
		TERM_ESCAPE TERM_RESET "\n", ##__VA_ARGS__)

/**
 * @brief Start a profiling section
 *
 * This macro helps with measuring the time it takes to complete
 * a particular section of code by inserting a measurement mechanism.
 */
#define LANE_PROFILING_START(section)	\
	struct timespec __start_##section , __end_##section ; \
	clock_gettime(CLOCK_REALTIME, &__start_##section ); \
	LANE_LOG_INFO("Entering section " #section);

/**
 * @brief End a profiling section
 *
 * This macro helps with measuring the time it takes to complete
 * a particular section of code by inserting a measurement mechanism.
 */
#define LANE_PROFILING_END(section)	\
	clock_gettime(CLOCK_REALTIME, &__end_##section ); \
	LANE_LOG_INFO("Completed section \"" #section "\" in %lf sec", \
			( __end_##section .tv_nsec - __start_##section .tv_nsec ) / 1e9 + \
			( __end_##section .tv_sec - __start_##section .tv_sec ));

/**
 * @brief Profile a function call, or a scoped section of code
 *
 * This macro helps with measuring the time it takes to complete
 * a certain function call by inserting a measurement mechanism.
 */
#define LANE_PROFILE(section, stmt)	\
	LANE_PROFILING_START(section)	\
	stmt;				\
	LANE_PROFILING_END(section)

/**
 * @internal
 *
 * The ASCII escape character for terminals
 */
#define TERM_ESCAPE			"\x1b["

/**
 * @internal
 */
#define TERM_COLOR_INFO			"30;2m"

/**
 * @internal
 */
#define TERM_COLOR_WARNING		"31m"

/**
 * @internal
 */
#define TERM_COLOR_FOREGROUND		"m"

/**
 * @internal
 */
#define TERM_RESET			"0m"

#else

#define LANE_LOG_INFO(format, ...)	({(void)0;})
#define LANE_LOG_ERROR(format, ...)	({(void)0;})

#endif /* LANE_LOG_ENABLE */

#endif /* LANE_LOG_H */
