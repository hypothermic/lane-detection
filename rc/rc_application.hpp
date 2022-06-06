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

/**
 * @see rc_information_window.hpp
 */
class InformationWindow;

/**
 * @see rc_uart_manager.hpp
 */
class UartManager;

/**
 * The main entry point of the program which handles the setup of
 * the user interface and the creation of the UART manager class.
 * It acts as a bridge between these by managing the data between
 * the two threads
 */
class RemoteControlApplication : public Gtk::Application {
	private:
		/**
		 * A reference to the main window. It is
		 * initialized upon activation of the application
		 */
		InformationWindow *info_window;

		/**
		 * Allows for messaging between the UI thread
		 * and the serial reader thread by emitting a
		 * signal when data has been receiver
		 */
		Glib::Dispatcher synchronizer;

		/**
		 * The system thread that the serial connection
		 * is accessed from
		 */
		std::thread *tty_thread;

		/**
		 * Wrapper for handling the serial communication
		 */
		UartManager *uart_manager;

	public:
		/*
		 * @inheritDoc
		 */
		RemoteControlApplication();


		// --- GTK event callbacks
		
		/**
		 * Called upon startup of the application
		 */
		void on_activate() override;

		/**
		 * Called when the user quits the application
		 */
		void on_destroy();

		// --- UI window action callbacks
	
		/**
		 * Called when the user requests to connect to
		 * a device at port <i>tty_name</i>
		 */
		void on_connect(Glib::ustring tty_name);
		
		/**
		 * Called when the user requests to disconnect
		 * from all devices
		 */
		void on_disconnect();

		// --- UART data thread callbacks

		/**
		 * Called when the state of the serial data thread
		 * has been updated. The new state can be read by
		 * claiming its mutex and reading its <i>state</i>
		 * member.
		 */
		void on_thread_sync();

		/**
		 * Called when the serial data receiver thread has
		 * received new data. The new data can be read by
		 * claiming its mutex and using its
		 * <i>get_new_packets</i> function.
		 */
		void on_thread_data_renew();
};

