/**
 * @file rc_log.hpp
 * @author Matthijs Bakker
 * @brief Logging utility functions
 */

#pragma once

#include <source_location>

#include <glibmm/ustring.h>

void rc_log_info(Glib::ustring message, const std::source_location& location = std::source_location::current());

void rc_log_error(Glib::ustring message, const std::source_location& location = std::source_location::current());

