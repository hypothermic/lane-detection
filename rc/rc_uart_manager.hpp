/**
 * @file rc_uart_manager.hpp
 * @author Matthijs Bakker
 * @brief A background thread that reads from UART using termios
 *
 * The UartManager class provides a method that can read a serial port on
 * a background thread and communicates back the results using shared data
 * and a mutex lock
 */

#pragma once

#include <mutex>
#include <queue>
#include <vector>

#include "rc_application.hpp"
#include "rc_connection.hpp"
#include "rc_uart_packet.hpp"

class RemoteControlApplication;

/**
 * Reads from a serial port on a background thread
 */
class UartManager {
	private:
		/**
		 * A mutex to make sure the variables never get read/written
		 * to at the same time
		 */
		mutable std::mutex mutex;

		/**
		 * The current connection state
		 */
		ConnectionState connection_state;

		/**
		 * The connection target device
		 */
		ConnectionTarget connection_target;

		/**
		 * FIFO queue with the packets that were received
		 */
		std::queue<UartPacket *> data;

		/**
		 * The GUI thread can set this to <i>true</i> to stop the
		 * worker thread
		 */
		bool stop_requested;

		/**
		 * The worker thread sets this to <i>true</i> if an error
		 * occurred with accessing the serial port
		 */
		bool tty_error;

	public:
		UartManager();

		void main_loop(RemoteControlApplication *parent_application);
		void request_stop();

		void set_connection_target(Glib::ustring target);
		ConnectionState get_connection_state();
		std::vector<UartPacket *> get_new_packets();
};

