#include <gtkmm/application.h>

#include "rc_application.hpp"

int main(int argc, char **argv) {
	auto app = Glib::make_refptr_for_instance<RemoteControlApplication>(new RemoteControlApplication());

	return app->run(argc, argv);
}

