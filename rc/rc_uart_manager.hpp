#pragma once

#include <mutex>

#include "rc_application.hpp"
#include "rc_connection.hpp"

class RemoteControlApplication;

class UartManager {
	private:
		mutable std::mutex mutex;
		ConnectionState connection_state;
		ConnectionTarget connection_target;
		bool data_available;
		bool stop_requested;
	public:
		UartManager();

		void main_loop(RemoteControlApplication *parent_application);
		void request_stop();

		ConnectionState get_connection_state();
};

