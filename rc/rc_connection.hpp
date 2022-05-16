/**
 * @file rc_connection.hpp
 * @author Matthijs Bakker
 * @brief Data types for passing connection parameters
 */

#pragma once

#include <glibmm/ustring.h>

enum class ConnectionState {
	DISCONNECTED,
	CONNECTED,
};

struct ConnectionTarget {
	Glib::ustring tty_port;
	//unsigned long baud_rate;
};

