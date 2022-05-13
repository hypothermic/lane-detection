#pragma once

#include <gtkmm/drawingarea.h>

#include "rc_uart_packet.hpp"

class PreviewArea : public Gtk::DrawingArea {
	private:
		FrameProcessedPacket::line_vector lines;

	public:
		PreviewArea();
		~PreviewArea();

		void on_frame_update(FrameProcessedPacket *packet);

	protected:
		void on_draw(const Cairo::RefPtr<Cairo::Context>& context, int width, int height);
};

