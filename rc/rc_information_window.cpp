/**
 * @file rc_information_window.cpp
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

#include "rc_information_window.hpp"

/*
 * @internal
 *
 * Set the label text with a value suffix
 *
 * @tparam T	the type of value which is supplied
 * 
 * @param label	the label which should be updated
 * @param name	the prefix of the label
 * @param value	the suffix of the label
 */
template<typename T>
static inline void set_label_text(Gtk::Label &label, const char *name, T value) {
	label.set_markup(Glib::ustring::sprintf("%s: <b>%s</b>", name, std::to_string(value)));
}

/*
 * @internal
 * @inheritDoc
 *
 * Specialization for booleans because <i>std::to_string</i> can't handle them...
 * Each day I'm losing more faith in the STL, it's almost as bad as the IEEE VHDL STD
 */
template<>
inline void set_label_text(Gtk::Label &label, const char *name, bool value) {
	label.set_markup(Glib::ustring::sprintf("%s: <b>%s</b>", name, (value ? "true" : "false")));
}

/*
 * @inheritDoc
 */
InformationWindow::InformationWindow(UartManager *uart_manager) :
	uart_manager(uart_manager),
	main_container(Gtk::Orientation::VERTICAL),
	connection_frame("Connection"),
	connect_button("Connect"),
	status_frame("Device status"),
	status_container(Gtk::Orientation::VERTICAL),
	preview_frame("Preview"),
	preview_aspect(Gtk::Align::CENTER, Gtk::Align::CENTER, (1280.0f/720.0f)),
	preview_area() {

	// Construct all the UI widgets.
	// This could be done using Glade in the future
	// It's an XML-like UI definition file that will help
	// replace the hardcoded stuff below here
	
	this->port_entry.set_text("/dev/ttyUSB0");
	this->port_entry.set_hexpand(true);

	// Connect the callback of the "Connect"/"Disconnect" button
	this->connect_button.signal_clicked().connect(
			sigc::mem_fun(*this, &InformationWindow::on_connect_request));

	this->connection_container.set_margin(CONTAINER_MARGIN);
	this->connection_container.append(this->port_entry);
	this->connection_container.append(this->connect_button);

	this->connection_frame.set_child(this->connection_container);

	// TODO use a for loop or macro or something

	this->is_processing_label.set_halign(Gtk::Align::START);
	this->is_processing_label.set_hexpand(false);
	this->is_processing_label.set_text("");
	this->seg_thres_label.set_halign(Gtk::Align::START);
	this->seg_thres_label.set_hexpand(false);
	this->seg_thres_label.set_text("");
	this->g_sigma_label.set_halign(Gtk::Align::START);
	this->g_sigma_label.set_hexpand(false);
	this->g_sigma_label.set_text("");
	this->e_thres_label.set_halign(Gtk::Align::START);
	this->e_thres_label.set_hexpand(false);
	this->e_thres_label.set_text("");
	this->h_thres_label.set_halign(Gtk::Align::START);
	this->h_thres_label.set_hexpand(false);
	this->h_thres_label.set_text("");
	this->departure_label.set_halign(Gtk::Align::START);
	this->departure_label.set_hexpand(false);
	this->departure_label.set_text("");

	this->status_container.set_margin(CONTAINER_MARGIN);
	this->status_container.append(this->is_processing_label);
	this->status_container.append(this->seg_thres_label);
	this->status_container.append(this->g_sigma_label);
	this->status_container.append(this->e_thres_label);
	this->status_container.append(this->h_thres_label);
	this->status_container.append(this->departure_label);

	this->status_frame.set_child(this->status_container);

	this->preview_area.set_expand(true);
	this->preview_aspect.set_child(this->preview_area);
	this->preview_aspect.set_expand(true);
	this->preview_frame.set_child(this->preview_aspect);

	this->main_container.set_margin(CONTAINER_MARGIN);
	this->main_container.append(this->connection_frame);
	this->main_container.append(this->status_frame);
	this->main_container.append(this->preview_frame);

	this->set_title("Remote Control");
	this->set_layout_manager(Gtk::BoxLayout::create(Gtk::Orientation::VERTICAL));
	this->set_child(this->main_container);
}

/*
 * @inheritDoc
 */
void InformationWindow::on_connection_state_update(ConnectionState state) {
	this->port_entry.set_editable(state == ConnectionState::DISCONNECTED);
	this->connect_button.set_label(state == ConnectionState::DISCONNECTED ? "Connect" : "Disconnect");
}

/*
 * @inheritDoc
 */
void InformationWindow::on_data_update(UartPacket *packet) {

	// Depending on which type of packet we have received,
	// we want to update the relevant information widgets
	switch (packet->get_type()) {

		// The packet contains a new status and parameters,
		// update the relevant fields in the Status frame;
		case static_cast<uint8_t>(UartPacket::Type::STATUS_UPDATE): {
			StatusUpdatePacket *sup = dynamic_cast<StatusUpdatePacket *>(packet);

			set_label_text<bool>(this->is_processing_label, "Is currently processing", sup->get_is_processing());
			set_label_text<int>(this->seg_thres_label, "Segmentation threshold", sup->get_seg_thres());
			set_label_text<float>(this->g_sigma_label, "Gaussian blur sigma (std. dev.)", sup->get_g_sigma());
			set_label_text<int>(this->e_thres_label, "Sobel filter edge threshold", sup->get_e_thres());
			set_label_text<int>(this->h_thres_label, "Hough accumulator vote threshold", sup->get_h_thres());
			set_label_text<bool>(this->departure_label, "Lane departure detected", false);

			break;
		}

		// The packet contains line data, delegate it to the
		// preview window so that it can plot the lines
		case static_cast<uint8_t>(UartPacket::Type::FRAME_PROCESSED): {
			this->preview_area.on_frame_update(dynamic_cast<FrameProcessedPacket *>(packet));

			break;
		}

		// The packet contains a boolean which indicates
		// whether the departure alarm has been activated.
		//
		// Update the status label and set the line plot
		// color to make the lines red/blue.
		case static_cast<uint8_t>(UartPacket::Type::DEPARTURE_WARNING): {
			DepartureWarningPacket *dwp = dynamic_cast<DepartureWarningPacket *>(packet);

			set_label_text<bool>(this->departure_label, "Lane departure detected", dwp->get_is_departing());

			if (dwp->get_is_departing()) {
				this->preview_area.set_line_color(LINE_COLOR_RED);
			} else {
				this->preview_area.set_line_color(LINE_COLOR_BLUE);
			}

			break;
		}
	}
}

/*
 * @inheritDoc
 */
void InformationWindow::on_connect_request() {

	// If the port text field is editable, the connection
	// hasn't been established, so we emit the connect signal.
	// Otherwise, emit the disconnect signal
	if (this->port_entry.get_editable() == true) {
		this->connect_request_signal(this->port_entry.get_text());
	} else {
		this->disconnect_request_signal();
	}
}

