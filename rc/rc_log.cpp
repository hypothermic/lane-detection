/**
 * @file rc_log.cpp
 * @author Matthijs Bakker
 * @brief Logging utility functions
 */

#include "rc_log.hpp"

#include <iostream>

static inline void rc_log(Glib::ustring message, const std::source_location& location, std::ostream& stream) {
	stream << location.file_name() << " @ " << location.line() << " - " << message << std::endl;
}
void rc_log_info(Glib::ustring message, const std::source_location& location) {
	rc_log(message, location, std::cout);
}

void rc_log_error(Glib::ustring message, const std::source_location& location) {
	rc_log(message, location, std::cerr);
}

