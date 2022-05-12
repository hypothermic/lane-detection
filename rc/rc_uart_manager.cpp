#include "rc_uart_manager.hpp"

#include <iostream>

UartManager::UartManager()
	: mutex(),
	  data_available(false),
	  stop_requested(false) {}

void UartManager::main_loop(RemoteControlApplication *parent_application) {
	{
		std::lock_guard<std::mutex> lock(this->mutex);
		this->data_available = false;
		this->stop_requested = false;
		this->connection_state = ConnectionState::CONNECTED;
	}

	std::cerr << "Connection thread start" << std::endl;
	parent_application->on_thread_sync();
	
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		{
			std::lock_guard<std::mutex> lock(mutex);

			if (this->stop_requested) {
				break;
			}
		}
	}

	std::cerr << "Connection thread stop" << std::endl;
	parent_application->on_thread_sync();
	
	{
		std::lock_guard<std::mutex> lock(this->mutex);
		this->connection_state = ConnectionState::DISCONNECTED;
	}
}

void UartManager::request_stop() {
	std::lock_guard<std::mutex> lock(this->mutex);

	this->stop_requested = true;
}

ConnectionState UartManager::get_connection_state() {
	std::lock_guard<std::mutex> lock(this->mutex);

	return this->connection_state;
}

