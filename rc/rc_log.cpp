/**
 * @file rc_log.cpp
 * @author Matthijs Bakker
 * @brief Logging utility functions
 */

#include "rc_log.hpp"

#include <iostream>

/**
 * Write a message to an output stream containing a formatted
 * text with the message and source file/line for easy debugging.
 *
 * @param message	The message to write
 * @param location	The source file/line where the log
 * 			statement was initiated from
 * @param stream	The output stream to write to
 */
static inline void rc_log(Glib::ustring message, const std::source_location& location, std::ostream& stream) {
	stream << location.file_name() << " @ " << location.line() << " - " << message << std::endl;
}

/*
 * @inheritDoc
 */
void rc_log_info(Glib::ustring message, const std::source_location& location) {
	rc_log(message, location, std::cout);
}

/*
 * @inheritDoc
 */
void rc_log_error(Glib::ustring message, const std::source_location& location) {
	rc_log(message, location, std::cerr);
}

