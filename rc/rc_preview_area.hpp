/**
 * @file rc_preview_area.hpp
 * @author Matthijs Bakker
 * @brief Subclass of <i>Gtk::PreviewArea</i> which draws Hough lines
 *
 * This class overrides the <i>on_draw</i> method of the preview area
 * and draws the detected lines on the widget using a Cairo context
 */

#pragma once

#include <gtkmm/drawingarea.h>

#include "rc_uart_packet.hpp"

#define LINE_COLOR_RED	1.0, 0.0, 0.0
#define LINE_COLOR_BLUE	0.0, 0.0, 1.0

class PreviewArea : public Gtk::DrawingArea {
	private:
		FrameProcessedPacket::line_vector lines;
		float r, g, b;

	public:
		PreviewArea();
		~PreviewArea();

		void set_line_color(float r, float g, float b);
		void on_frame_update(FrameProcessedPacket *packet);

	protected:
		void on_draw(const Cairo::RefPtr<Cairo::Context>& context, int width, int height);
};

