/**
 * @file rc_application.cpp
 * @author Matthijs Bakker
 * @brief Subclass of <i>Gtk::Application</i> which handles app events
 *
 * This unit handles sychronization between the serial reader thread
 * and the information window
 */

#include "rc_application.hpp"

#include "rc_uart_packet.hpp"

/*
 * @inheritDoc
 */
RemoteControlApplication::RemoteControlApplication()
	: Gtk::Application("com.arobs.lane.RemoteControl"),
	  synchronizer(),
	  tty_thread(nullptr),
	  uart_manager(new UartManager()) {

	// Register the callback for when UART Data has been received
  	this->synchronizer.connect(sigc::mem_fun(*this, &RemoteControlApplication::on_thread_data_renew));
}

/*
 * @inheritDoc
 */
void RemoteControlApplication::on_activate() {
	auto info_window = new InformationWindow(uart_manager);

	this->add_window(*info_window);
	this->info_window = info_window;	

	// Register the callback for when the user quits the application
	info_window->signal_hide().connect(sigc::mem_fun(*this, &RemoteControlApplication::on_destroy));

	// Register the callback for when the user clicks the "Connect" button
	info_window->connect_request_signal.connect(sigc::mem_fun(*this, &RemoteControlApplication::on_connect));

	// Register the callback for when the user clicks the "Disconnect" button
	info_window->disconnect_request_signal.connect(sigc::mem_fun(*this, &RemoteControlApplication::on_disconnect));

	// Show the window on the screen
	info_window->present();
}

/*
 * @inheritDoc
 */
void RemoteControlApplication::on_destroy() {
	delete this->info_window;
}

/*
 * @inheritDoc
 */
void RemoteControlApplication::on_connect(Glib::ustring tty_name) {
	uart_manager->set_connection_target(tty_name);

	// Run the UART receive loop on a new thread
	this->tty_thread = new std::thread([this] {
		uart_manager->main_loop(this);
	});
}

/*
 * @inheritDoc
 */
void RemoteControlApplication::on_disconnect() {
	this->uart_manager->request_stop();
}

/*
 * @inheritDoc
 */
void RemoteControlApplication::on_thread_sync() {
	this->synchronizer.emit();
}

/*
 * @inheritDoc
 */
void RemoteControlApplication::on_thread_data_renew() {
	
	// This code is run on the UI thread and we should have acquired the mutex (check the calling code to be sure)
	// So we can read the UartManager's packets and connection state

	std::vector<UartPacket *> packets = this->uart_manager->get_new_packets();

	this->info_window->on_connection_state_update(this->uart_manager->get_connection_state());

	// Send every packet that was received in this burst to the information window
	for (UartPacket *packet : packets) {
		this->info_window->on_data_update(packet);

		// And then free the memory since we took ownership of it
		delete packet;
	}
}

