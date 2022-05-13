#include "rc_information_window.hpp"

#include <iostream>

static inline void set_label_text(Gtk::Label &label, const char *name, int value) {
	Glib::ustring seg_thres_str;

	label.set_markup(Glib::ustring::sprintf("%s: <b>%d</b>", name, value));
}

InformationWindow::InformationWindow(UartManager *uart_manager) :
	uart_manager(uart_manager),
	main_container(Gtk::Orientation::VERTICAL),
	connection_frame("Connection"),
	connect_button("Connect"),
	status_frame("Device status"),
	status_container(Gtk::Orientation::VERTICAL) {

	this->port_entry.set_text("/dev/ttyUSB0");
	this->port_entry.set_hexpand(true);

	this->connect_button.signal_clicked().connect(
			sigc::mem_fun(*this, &InformationWindow::on_connect_request));

	this->connection_container.set_margin(CONTAINER_MARGIN);
	this->connection_container.append(this->port_entry);
	this->connection_container.append(this->connect_button);

	this->connection_frame.set_child(this->connection_container);

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

	this->status_container.set_margin(CONTAINER_MARGIN);
	this->status_container.append(this->is_processing_label);
	this->status_container.append(this->seg_thres_label);
	this->status_container.append(this->g_sigma_label);
	this->status_container.append(this->e_thres_label);
	this->status_container.append(this->h_thres_label);

	this->status_frame.set_child(this->status_container);

	this->main_container.set_margin(CONTAINER_MARGIN);
	this->main_container.append(this->connection_frame);
	this->main_container.append(this->status_frame);

	this->set_layout_manager(Gtk::BoxLayout::create(Gtk::Orientation::VERTICAL));
	this->set_child(this->main_container);
}

void InformationWindow::on_connection_state_update(ConnectionState state) {
	this->port_entry.set_editable(state == ConnectionState::DISCONNECTED);
	this->connect_button.set_label(state == ConnectionState::DISCONNECTED ? "Connect" : "Disconnect");
}

void InformationWindow::on_data_update(UartPacket *packet) {
	switch (packet->get_type()) {
		case static_cast<uint8_t>(UartPacket::Type::STATUS_UPDATE):
			StatusUpdatePacket *sup = dynamic_cast<StatusUpdatePacket *>(packet);
			set_label_text(this->is_processing_label, "Is currently processing", static_cast<int>(sup->get_is_processing()));
			set_label_text(this->seg_thres_label, "Segmentation threshold", static_cast<int>(sup->get_seg_thres()));
			set_label_text(this->g_sigma_label, "Gaussian blur sigma (std. dev.)", static_cast<int>(sup->get_g_sigma()));
			set_label_text(this->e_thres_label, "Sobel filter edge threshold", static_cast<int>(sup->get_e_thres()));
			set_label_text(this->h_thres_label, "Hough accumulator vote threshold", static_cast<int>(sup->get_h_thres()));

			break;
	}
}

void InformationWindow::on_connect_request() {
	if (this->port_entry.get_editable() == true) {
		this->connect_request_signal(this->port_entry.get_text());
	} else {
		this->disconnect_request_signal();
	}
}

