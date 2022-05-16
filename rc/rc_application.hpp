/**
 * @file rc_application.hpp
 * @author Matthijs Bakker
 * @brief Subclass of <i>Gtk::Application</i> which handles app events
 *
 * This unit handles sychronization between the serial reader thread
 * and the information window
 */

#pragma once

#include <thread>

#include <glibmm/dispatcher.h>
#include <glibmm/ustring.h>
#include <gtkmm/application.h>

#include "rc_information_window.hpp"
#include "rc_uart_manager.hpp"

class InformationWindow;
class UartManager;

class RemoteControlApplication : public Gtk::Application {
	private:
		InformationWindow *info_window;

		// Messaging between threads
		Glib::Dispatcher synchronizer;
		std::thread *tty_thread;
		UartManager *uart_manager;

	public:
		RemoteControlApplication();


		// GTK event callbacks
		void on_activate() override;
		void on_destroy();

		// UI connect button callback
		void on_connect(Glib::ustring tty_name);
		void on_disconnect();

		// UART thread callbacks
		void on_thread_sync();
		void on_thread_data_renew();
};

