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
		/**
		 * Construct a new instance and initialize it with defaults
		 */
		UartManager();

		/**
		 * Run a blocking main loop which exits upon error or
		 * upon a stop request
		 *
		 * @param parent	The application that should be
		 * 			informed of events
		 */
		void main_loop(RemoteControlApplication *parent_application);

		/**
		 * Send a stop request to the thread's main loop.
		 *
		 * This function is thread-safe
		 */
		void request_stop();

		/**
		 * Set the path to the virtual serial device.
		 *
		 * NOT threadsafe, call before the mainloop is started
		 */
		void set_connection_target(Glib::ustring target);

		/**
		 * Get the current state of the connection.
		 *
		 * This function is thread-safe
		 */
		ConnectionState get_connection_state();

		/**
		 * Retrieve new packets from the receive buffer and
		 * place them in a newly created vector.
		 *
		 * Ownership of all packet references is delegated to
		 * the caller function, thus it should free the data.
		 *
		 * This function is thread-safe
		 */
		std::vector<UartPacket *> get_new_packets();
};

