/**
 * @file rc_uart_manager.cpp
 * @author Matthijs Bakker
 * @brief A background thread that reads from UART using termios
 *
 * The UartManager class provides a method that can read a serial port on
 * a background thread and communicates back the results using shared data
 * and a mutex lock.
 */

#include "rc_uart_manager.hpp"

#include <iomanip>

/*
 * Headers needed for serial port communication
 *
 * The errno, fcntl, termios and unistd are UNIX-specific, so if
 * we were to port this application to Windows or other platforms
 * it would be wise to create some kind of abstraction
 */
#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "rc_log.hpp"
#include "rc_uart_packet.hpp"

/*
 * @inheritDoc
 */
UartManager::UartManager()
	: mutex(),
	  data(),
	  stop_requested(false),
	  tty_error(false) {}

/*
 * @inheritDoc
 */
void UartManager::main_loop(RemoteControlApplication *parent_application) {
	/**
	 * UART connection settings struct
	 */
	struct termios tty = {0};
	/**
	 * Absolute path to the virtual serial device
	 */
	const char *file;
	/**
	 * File descriptor (handle) to the virtual serial device
	 */
	int fd;

	// Reset the internal state and signal that we are connected
	{
		std::lock_guard<std::mutex> lock(this->mutex);
		this->stop_requested = false;
		this->connection_state = ConnectionState::CONNECTED;
		file = this->connection_target.tty_port.c_str();
	}

	// Communicate with the application that we updated our state
	parent_application->on_thread_sync();

	// Try opening the port, get a file descriptor
	fd = open(file, O_RDWR);

	// Unable to open the serial port, indicate an error
	if (fd < 0) {
		{
			std::lock_guard<std::mutex> lock(this->mutex);
			this->tty_error = true;
			this->connection_state = ConnectionState::DISCONNECTED;
		}

		rc_log_error("Unable to open the serial port");
		parent_application->on_thread_sync();
		return;
	}
	
	// Configure the UART parameters
        tty.c_iflag = 0;
        tty.c_oflag = 0;
        tty.c_cflag = CS8 | CREAD | CLOCAL | PARENB | PARODD; // 8O1, 8 data bits, odd parity bit
        tty.c_lflag = 0;

        // Block for 0.1 sec regardless of data received
	tty.c_cc[VMIN] = 0;
        tty.c_cc[VTIME] = 1;

	// Set baud rate of 230400
	cfsetispeed(&tty, B230400);
	cfsetospeed(&tty, B230400);

	// Apply settings
	tcsetattr(fd, TCSANOW, &tty);

	// Loop until user disconnects or port disconnects
	while (true) {
		/*
		 * Temporary buffer to read our data into
		 */
		char buffer[1024] = {0};
		/*
		 * The amount of bytes read
		 */
		int num_read = 0;
		/*
		 * The index of the byte that we are parsing
		 */
		int current = 0;

		/*
		 * Read data from the virtual serial device
		 * This will block for X amount of time (see above)
		 * and it will return the amount of bytes read
		 */
		num_read = read(fd, &buffer, sizeof(buffer));

		// Check for error or port closure
		if (num_read < 0) {
			// Update our state, indicating that we errored
			{
				std::lock_guard<std::mutex> lock(this->mutex);
				this->tty_error = true;
				this->connection_state = ConnectionState::DISCONNECTED;
			}

			rc_log_error("Port closed with error");
			break;
		}

		// Read all the bytes that we received
		// Keep reading packets until we processed all data
		while (current < num_read) {
			std::lock_guard<std::mutex> lock(this->mutex);
			UartPacket *packet = nullptr;

			//std::cerr << "Current: " << current << " Total: " << num_read << " Of buffer: ";
			//for (int i = 0; i < num_read; ++i) {
			//	std::cerr << std::to_string((uint8_t) buffer[i]) << " ";
			//}
			//std::cerr << std::endl;

			current = UartPacket::read(buffer, current, &packet);

			if (packet) {
				data.push(packet);
			}
		}

		// Check if the parent application has requested us to stop
		{
			std::lock_guard<std::mutex> lock(mutex);

			if (this->stop_requested) {
				break;
			}
		}

		// If we received new data, send a signal to the application
		if (!this->data.empty()) {
			parent_application->on_thread_sync();
		}
	}

	close(fd);
	
	// Update our state, indicating that we have disconnected
	{
		std::lock_guard<std::mutex> lock(this->mutex);
		this->connection_state = ConnectionState::DISCONNECTED;
	}

	// Notify the parent application of our state change
	parent_application->on_thread_sync();
}

/*
 * @inheritDoc
 */
void UartManager::request_stop() {
	std::lock_guard<std::mutex> lock(this->mutex);

	this->stop_requested = true;
}

/*
 * @inheritDoc
 */
void UartManager::set_connection_target(Glib::ustring target) {
	this->connection_target.tty_port = target;
}

/*
 * @inheritDoc
 */
ConnectionState UartManager::get_connection_state() {
	std::lock_guard<std::mutex> lock(this->mutex);

	return this->connection_state;
}

/*
 * @inheritDoc
 */
std::vector<UartPacket *> UartManager::get_new_packets() {
	std::lock_guard<std::mutex> lock(this->mutex);
	std::vector<UartPacket *> result;

	// Move all elements from the receiver vector to
	// the newly created vector
	while (!this->data.empty()) {
		result.push_back(this->data.front());
		this->data.pop();
	}

	return result;
}

