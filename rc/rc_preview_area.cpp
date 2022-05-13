#include "rc_preview_area.hpp"

#include <cmath>

PreviewArea::PreviewArea() {
	this->set_draw_func(sigc::mem_fun(*this, &PreviewArea::on_draw));
}

PreviewArea::~PreviewArea() {}

static inline void draw_line(const Cairo::RefPtr<Cairo::Context> &context, const std::pair<float, float> &line, int c_width, int c_height) {
	constexpr int D = 1500;
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

void PreviewArea::on_frame_update(FrameProcessedPacket *packet) {
	this->lines = packet->get_lines();

	this->queue_draw();
}

void PreviewArea::on_draw(const Cairo::RefPtr<Cairo::Context> &context, int width, int height) {
	// Fill background so we can tell it apart from the other widgets
	context->save();
	context->set_source_rgba(0.0, 0.0, 0.0, 0.3);
	context->paint();
	context->restore();

	// Draw all the Hough lines
	context->save();
	context->translate(0.5f, 0.0f);
	context->set_line_width(5.0f);
	context->set_source_rgba(1.0, 0.0, 0.0, 1.0);

	for (auto line : this->lines) {
		draw_line(context, line, width, height);
	}

	context->stroke();
	context->fill_preserve();
	context->stroke_preserve();
	context->clip();
	context->restore();
}

