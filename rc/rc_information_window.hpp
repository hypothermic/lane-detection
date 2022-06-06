/**
 * @file rc_information_window.hpp
 * @author Matthijs Bakker
 * @brief The GUI window which displays received information
 *
 * This window has three parts:
 *   - the connection settings with the connect button
 *   - the algorithm parameters of the lane detector
 *   - a simulated Hough lines preview
 *
 * All data is provided to the window by the application class
 */

#pragma once

#include <gtkmm/applicationwindow.h>
#include <gtkmm/aspectframe.h>
#include <gtkmm/box.h>
#include <gtkmm/boxlayout.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>

#include "rc_connection.hpp"
#include "rc_preview_area.hpp"
#include "rc_uart_manager.hpp"
#include "rc_uart_packet.hpp"

/**
 * @see rc_connection.hpp
 */
enum class ConnectionState;

/**
 * @see rc_uart_manager.hpp
 */
class UartManager;

/**
 * This window lets the user connect with the device and
 * displays the received data in a formatted manner.
 */
class InformationWindow : public Gtk::ApplicationWindow {

	/**
	 * How many pixels around the containers should be kept free
	 */
	const int CONTAINER_MARGIN = 4;

	private:
		/**
		 * A reference to the app's UART connection manager
		 */
		const UartManager *uart_manager;

		/**
		 * The container that holds all of the frames
		 */
		Gtk::Box main_container;

		// --- "Connection" frame

		/**
		 * The frame which holds the widgets regarding
		 * the connection info and the connect/disconnect
		 * buttons
		 */
		Gtk::Frame connection_frame;
		Gtk::Box connection_container;
		
		/**
		 * A text field which allows the user to enter
		 * the path to the virtual serial device
		 */
		Gtk::Entry port_entry;

		/**
		 * A button which sends the connection request
		 * to the RemoteControlApplication object
		 */
		Gtk::Button connect_button;

		// --- "Status" frame

		/**
		 * The frame with device status info and
		 * algorithm parameter info
		 */
		Gtk::Frame status_frame;
		Gtk::Box status_container;

		Gtk::Label is_processing_label,
			   seg_thres_label,
			   g_sigma_label,
			   e_thres_label,
			   h_thres_label,
			   departure_label;

		// --- "Preview" frame

		/**
		 * The frame which holds the widgets related
		 * to the live preview feed
		 */
		Gtk::Frame preview_frame;

		/**
		 * This frame makes sure that the aspect ratio
		 * of its child (the PreviewArea drawing area)
		 * stays at 16:9 (or whatever the dashcam feed
		 * aspect ratio is...)
		 */
		Gtk::AspectFrame preview_aspect;

		/**
		 * A drawing area that has a dark background
		 * and plots the lines that were received
		 */
		PreviewArea preview_area;

	public:
		/**
		 * A signal which is activated when the user
		 * presses the "Connect" button.
		 * 
		 * It sends a string with the user specified
		 * path to the virtual serial device
		 */
		sigc::signal<void(Glib::ustring)> connect_request_signal;
		
		/**
		 * A signal which is activated when the user
		 * presses the "Connect" button.
		 */
		sigc::signal<void()> disconnect_request_signal;

		/*
		 * @inheritDoc
		 */
		InformationWindow(UartManager *uart_manager);

		// --- Application callbacks

		/**
		 * Called when the state of the connection
		 * has been changed
		 */
		void on_connection_state_update(ConnectionState state);

		/**
		 * Called when there is new data to display
		 */
		void on_data_update(UartPacket *packet);

	private:
		// --- UI callbacks
		
		/**
		 * Called when the user clicks the "Connect"
		 * button
		 */
		void on_connect_request();
};

