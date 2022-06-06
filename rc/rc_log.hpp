/**
 * @file rc_log.hpp
 * @author Matthijs Bakker
 * @brief Logging utility functions
 */

#pragma once

#include <source_location>

#include <glibmm/ustring.h>

/**
 * Write an information message to the standard output
 *
 * @param message	The message to print
 * @param location	The current line in the source file
 */
void rc_log_info(Glib::ustring message, const std::source_location& location = std::source_location::current());

/**
 * Write an error message to the standard error output
 *
 * @param message	The message to print
 * @param location	The current line in the source file
 */
void rc_log_error(Glib::ustring message, const std::source_location& location = std::source_location::current());

