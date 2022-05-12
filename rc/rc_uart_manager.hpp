#pragma once

#include <mutex>
#include <queue>
#include <vector>

#include "rc_application.hpp"
#include "rc_connection.hpp"
#include "rc_uart_packet.hpp"

class RemoteControlApplication;

class UartManager {
	private:
		mutable std::mutex mutex;
		ConnectionState connection_state;
		ConnectionTarget connection_target;
		std::queue<UartPacket *> data;
		bool stop_requested;
		bool tty_error;

	public:
		UartManager();

		void main_loop(RemoteControlApplication *parent_application);
		void request_stop();

		void set_connection_target(Glib::ustring target);
		ConnectionState get_connection_state();
		std::vector<UartPacket *> get_new_packets();
};

