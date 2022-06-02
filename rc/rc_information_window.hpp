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

class UartManager;
enum class ConnectionState;

class InformationWindow : public Gtk::ApplicationWindow {

	// How much pixels around the containers should be free
	const int CONTAINER_MARGIN = 4;

	private:
		// A reference to the app's UART connection manager
		const UartManager *uart_manager;

		// The container that holds all of the frames
		Gtk::Box main_container;

		// The frame with connection info and
		// connect/disconnect buttons
		Gtk::Frame connection_frame;
		Gtk::Box connection_container;
		Gtk::Entry port_entry;
		Gtk::Button connect_button;

		// Status info and algorithm parameter info
		Gtk::Frame status_frame;
		Gtk::Box status_container;
		Gtk::Label is_processing_label,
			   seg_thres_label,
			   g_sigma_label,
			   e_thres_label,
			   h_thres_label,
			   departure_label;

		// Preview lines etc
		Gtk::Frame preview_frame;
		Gtk::AspectFrame preview_aspect;
		PreviewArea preview_area;

	public:
		sigc::signal<void(Glib::ustring)> connect_request_signal;
		sigc::signal<void()> disconnect_request_signal;

		InformationWindow(UartManager *uart_manager);

		// Application callbacks
		void on_connection_state_update(ConnectionState state);
		void on_data_update(UartPacket *packet);

		// UI callbacks
		void on_connect_request();
};
