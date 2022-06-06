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

/**
 * A widget which plots the Hough lines onto it.
 *
 * The conversion from rho/theta to the internal
 * format is handled in the draw method, this
 * should probably not be done on every draw and
 * should be cached somewhere... TODO
 */
class PreviewArea : public Gtk::DrawingArea {
	private:
		/**
		 * The lines that are currently displayed
		 * on the preview area
		 */
		FrameProcessedPacket::line_vector lines;

		/**
		 * The color component weights of the
		 * current plotting color
		 */
		float r, g, b;

	public:
		/*
		 * @inheritDoc
		 */
		PreviewArea();
		
		/*
		 * @inheritDoc
		 */
		~PreviewArea();

		/*
		 * Set the color in which the lines are plotted
		 *
		 * @param r, g, b	The line color
		 */
		void set_line_color(float r, float g, float b);

		/**
		 * Called when new line data has been received
		 *
		 * @param packet	New line data
		 */
		void on_frame_update(FrameProcessedPacket *packet);

	protected:
		/**
		 * Called by GTK when the widget should be redrawn
		 *
		 * @param context	The drawing context
		 * @param width		The width of the widget
		 * @param height	The height of the widget
		 */
		void on_draw(const Cairo::RefPtr<Cairo::Context>& context, int width, int height);
};

