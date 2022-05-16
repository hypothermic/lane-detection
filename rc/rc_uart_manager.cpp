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

// Needed for serial port communication
#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <iomanip>

#include "rc_log.hpp"
#include "rc_uart_packet.hpp"

UartManager::UartManager()
	: mutex(),
	  data(),
	  stop_requested(false),
	  tty_error(false) {}

void UartManager::main_loop(RemoteControlApplication *parent_application) {
	struct termios tty = {0};
	const char *file;
	int fd;

	{
		std::lock_guard<std::mutex> lock(this->mutex);
		this->stop_requested = false;
		this->connection_state = ConnectionState::CONNECTED;
		file = this->connection_target.tty_port.c_str();
	}

	parent_application->on_thread_sync();

	// Try opening the port, get a file descriptor
	fd = open(file, O_RDWR);

	// Unable to open the serial port
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
		char buffer[1024] = {0};
		int num_read = 0,
		    current = 0;

		num_read = read(fd, &buffer, sizeof(buffer));

		// Check for error or port closure
		if (num_read < 0) {
			{
				std::lock_guard<std::mutex> lock(this->mutex);
				this->tty_error = true;
				this->connection_state = ConnectionState::DISCONNECTED;
			}

			rc_log_error("Port closed with error");
			break;
		}

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

		{
			std::lock_guard<std::mutex> lock(mutex);

			if (this->stop_requested) {
				break;
			}
		}

		if (!this->data.empty()) {
			parent_application->on_thread_sync();
		}
	}

	close(fd);
	
	{
		std::lock_guard<std::mutex> lock(this->mutex);
		this->connection_state = ConnectionState::DISCONNECTED;
	}

	parent_application->on_thread_sync();
}

void UartManager::request_stop() {
	std::lock_guard<std::mutex> lock(this->mutex);

	this->stop_requested = true;
}

void UartManager::set_connection_target(Glib::ustring target) {
	this->connection_target.tty_port = target;
}

ConnectionState UartManager::get_connection_state() {
	std::lock_guard<std::mutex> lock(this->mutex);

	return this->connection_state;
}

std::vector<UartPacket *> UartManager::get_new_packets() {
	std::lock_guard<std::mutex> lock(this->mutex);
	std::vector<UartPacket *> result;

	while (!this->data.empty()) {
		result.push_back(this->data.front());
		this->data.pop();
	}

	return result;
}

