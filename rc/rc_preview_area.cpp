/**
 * @file rc_preview_area.cpp
 * @author Matthijs Bakker
 * @brief Subclass of <i>Gtk::PreviewArea</i> which draws Hough lines
 *
 * This class overrides the <i>on_draw</i> method of the preview area
 * and draws the detected lines on the widget using a Cairo context
 */

#include "rc_preview_area.hpp"

/*
 * cmath is included for sinf() and cosf()
 */
#include <cmath>

/*
 * Yes, C++ people use constants, but LET ME HAVE MY DEFINES !!
 *
 * These are for red/blue in RGB float triplets
 */
#define LINE_COLOR_RED	1.0, 0.0, 0.0
#define LINE_COLOR_BLUE	0.0, 0.0, 1.0

/*
 * @inheritDoc
 */
PreviewArea::PreviewArea() {
	this->set_draw_func(sigc::mem_fun(*this, &PreviewArea::on_draw));
	this->set_line_color(LINE_COLOR_BLUE);
}

/*
 * @inheritDoc
 */
PreviewArea::~PreviewArea() {}

/*
 * Calculates the line from rho,theta format to x1,y1,x2,y2 edge format.
 *
 * It also plots the line using the new coordinates onto the Cairo context
 */
static inline void draw_line(const Cairo::RefPtr<Cairo::Context> &context, const std::pair<float, float> &line, int c_width, int c_height) {
	/*
	 * Diagonal value of the image
	 */
	constexpr int D = 1500;
	
	/*
	 * The math below is explained in the research paper
	 */
	float width = c_width / 2,
	      height = c_height / 2,
	      rho = line.first,
	      theta = line.second,
	      a = cosf(theta),
	      b = sinf(theta),
	      c = a * rho,
	      d = b * rho;
	int x1 = c + D * (-b) + width,
	    y1 = d + D * a + height,
	    x2 = c - D * (-b) + width,
	    y2 = d - D * a + height;

	context->move_to(x1, y1);
	context->line_to(x2, y2);
}

/*
 * @inheritDoc
 */
void PreviewArea::set_line_color(float r, float g, float b) {
	this->r = r;
	this->g = g;
	this->b = b;
}

/*
 * @inheritDoc
 */
void PreviewArea::on_frame_update(FrameProcessedPacket *packet) {
	this->lines = packet->get_lines();

	this->queue_draw();
}

/*
 * @inheritDoc
 */
void PreviewArea::on_draw(const Cairo::RefPtr<Cairo::Context> &context, int width, int height) {
	/*
	 * Fill the widget's background so we can
	 * tell it apart from the other widgets
	 */
	context->save();
	context->set_source_rgba(0.0, 0.0, 0.0, 0.3);
	context->paint();
	context->restore();

	/*
	 * Draw all the Hough lines
	 */
	context->save();
	context->translate(0.5f, 0.0f);
	context->set_line_width(5.0f);
	context->set_source_rgba(r, g, b, 1.0);

	for (auto line : this->lines) {
		draw_line(context, line, width, height);
	}

	context->stroke();

	/*
	 * This makes sure that the lines are
	 * drawn OVER the background, not below it
	 */
	context->fill_preserve();
	context->stroke_preserve();
	context->clip();
	context->restore();
}

