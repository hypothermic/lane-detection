#include "rc_application.hpp"

#include <iostream>

#include "rc_uart_packet.hpp"

RemoteControlApplication::RemoteControlApplication()
	: Gtk::Application("com.arobs.lane.RemoteControl"),
	  synchronizer(),
	  tty_thread(nullptr),
	  uart_manager(new UartManager()) {
  	this->synchronizer.connect(sigc::mem_fun(*this, &RemoteControlApplication::on_thread_data_renew));
}

void RemoteControlApplication::on_activate() {
	auto info_window = new InformationWindow(uart_manager);

	this->add_window(*info_window);

	info_window->signal_hide().connect(sigc::mem_fun(*this, &RemoteControlApplication::on_destroy));
	info_window->connect_request_signal.connect(sigc::mem_fun(*this, &RemoteControlApplication::on_connect));
	info_window->disconnect_request_signal.connect(sigc::mem_fun(*this, &RemoteControlApplication::on_disconnect));
	info_window->present();

	this->info_window = info_window;	
}

void RemoteControlApplication::on_destroy() {
	delete this->info_window;
}

void RemoteControlApplication::on_connect(Glib::ustring tty_name) {
	std::cerr << "connecting: " << tty_name << std::endl;

	uart_manager->set_connection_target(tty_name);

	this->tty_thread = new std::thread([this] {
		uart_manager->main_loop(this);
	});
}

void RemoteControlApplication::on_disconnect() {
	std::cerr << "TODO disconnect " << std::endl;
	this->uart_manager->request_stop();
}

void RemoteControlApplication::on_thread_data_renew() {
	std::cerr << "state update " << std::endl;
	this->info_window->on_connection_state_update(this->uart_manager->get_connection_state());
	std::vector<UartPacket *> packets = this->uart_manager->get_new_packets();

	for (UartPacket *packet : packets) {
		this->info_window->on_data_update(packet);

		delete packet;
	}
}

void RemoteControlApplication::on_thread_sync() {
	this->synchronizer.emit();
}

