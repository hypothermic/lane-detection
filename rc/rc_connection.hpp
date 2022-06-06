/**
 * @file rc_connection.hpp
 * @author Matthijs Bakker
 * @brief Data types for passing connection parameters
 */

#pragma once

#include <glibmm/ustring.h>

/**
 * The current state of a connection
 */
enum class ConnectionState {
	DISCONNECTED,
	CONNECTED,
};

/**
 * The parameters used for connecting to a target device
 */
struct ConnectionTarget {
	Glib::ustring tty_port;
	//unsigned long baud_rate;
};

