//#include <gtkmm.h>
#include <gtkmm/application.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/boxlayout.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>

class UartManager {
	public:
		UartManager();

		using data_received_signal = sigc::signal<void()>;
};

class InformationWindow : public Gtk::ApplicationWindow {
	const int CONTAINER_MARGIN = 4;

	private:
		Gtk::Box main_container;

		// The frame with connection info and
		// connect/disconnect buttons
		Gtk::Frame connection_frame;
		Gtk::Box connection_container;
		Gtk::Entry port_entry;
		Gtk::Label port_label;

	public:
		InformationWindow();
};

InformationWindow::InformationWindow() :
	main_container(Gtk::Orientation::VERTICAL),
	connection_frame("Connection"),
	port_label("Not connected") {

	this->set_layout_manager(Gtk::BoxLayout::create(Gtk::Orientation::VERTICAL));

	this->port_entry.set_max_length(32);
	this->port_entry.set_text("/dev/ttyUSB0");
	this->port_entry.set_hexpand(true);

	this->connection_container.set_margin(CONTAINER_MARGIN);
	this->connection_container.append(this->port_entry);
	this->connection_container.append(this->port_label);

	this->connection_frame.set_child(this->connection_container);

	this->main_container.set_margin(CONTAINER_MARGIN);
	this->main_container.append(this->connection_frame);
	this->set_child(this->main_container);
}

class RemoteControlApplication : public Gtk::Application {
	private:
		InformationWindow *info_window;
	// Class interface
	public:
		RemoteControlApplication();

	// GTK event callbacks
	protected:
		void on_activate() override;
		void on_destroy();
};

RemoteControlApplication::RemoteControlApplication() : Gtk::Application("com.arobs.lane.RemoteControl") {}

void RemoteControlApplication::on_activate() {
	auto info_window = new InformationWindow();

	this->add_window(*info_window);
	
	info_window->signal_hide().connect(sigc::mem_fun(*this, &RemoteControlApplication::on_destroy));
	info_window->present();

	this->info_window = info_window;
}

void RemoteControlApplication::on_destroy() {
	delete this->info_window;
}

int main(int argc, char **argv) {
	auto app = Glib::make_refptr_for_instance<RemoteControlApplication>(new RemoteControlApplication());

	return app->run(argc, argv);
}

