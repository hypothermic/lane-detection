/**
 * @file rc_uart_packet.cpp
 * @author Matthijs Bakker
 * @brief Abstraction of the different packet types
 *
 * The UartPacket class and its subclasses provide an abstraction layer
 * over the reading of data messages over the serial port
 */

#include "rc_uart_packet.hpp"

#include <type_traits>

#include "rc_log.hpp"

/**
 * Converts a fixed point number to floating point
 *
 * @tparam F	The amount of fractional bits to use
 *
 * @param fixed	The fixed-point number to convert
 */
template<int F>
static inline float fixed_to_float(uint8_t fixed) {
	constexpr uint8_t divisor = (1 << F);

	return (float) fixed / divisor;
}

UartPacket::UartPacket() {}

UartPacket::~UartPacket() {}

size_t UartPacket::read(char *buffer, size_t offset, UartPacket **packet) {
	UartPacket *result = nullptr;

	// Instantiate the packet class depending on which kind of packet this is
	// There must be some better way to do this in cpp but whatever...
	switch (buffer[offset]) {
		case static_cast<uint8_t>(UartPacket::Type::STATUS_UPDATE):
			result = new StatusUpdatePacket();
			break;
		case static_cast<uint8_t>(UartPacket::Type::FRAME_PROCESSED):
			result = new FrameProcessedPacket();
			break;
		case static_cast<uint8_t>(UartPacket::Type::DEPARTURE_WARNING):
			result = new DepartureWarningPacket();
			break;
		default:
			rc_log_error("Unknown packet type");
			return offset + 1;
	}

	// Read the packet data
	offset = result->read(buffer, offset);

	// Return it to the caller function by assigning the dest pointer
	(*packet) = result;

	return offset;
}

size_t UartPacket::read(char *buffer, size_t offset) {
	type = buffer[offset];

	return offset + 1;
}

uint8_t UartPacket::get_type() {
	return this->type;
}

size_t UartPacket::get_length() {
	return 1;
}

StatusUpdatePacket::StatusUpdatePacket() : UartPacket() {};

StatusUpdatePacket::~StatusUpdatePacket() {};

size_t StatusUpdatePacket::read(char *buffer, size_t offset) {
	offset = UartPacket::read(buffer, offset);

	// TODO check for buffer overflow
	
	is_processing = buffer[offset] != 0;
	seg_thres = buffer[offset + 1];
	g_sigma = fixed_to_float<6>(buffer[offset + 2]);
	e_thres = buffer[offset + 3];
	h_thres = buffer[offset + 4];

	return offset + 5;
}

size_t StatusUpdatePacket::get_length() {
	return 5 + UartPacket::get_length();
}

bool StatusUpdatePacket::get_is_processing() {
	return this->is_processing;
}

uint8_t StatusUpdatePacket::get_seg_thres() {
	return this->seg_thres;
}

float StatusUpdatePacket::get_g_sigma() {
	return this->g_sigma;
}

uint8_t StatusUpdatePacket::get_e_thres() {
	return this->e_thres;
}

uint8_t StatusUpdatePacket::get_h_thres() {
	return this->h_thres;
}

FrameProcessedPacket::FrameProcessedPacket()
	: UartPacket(), lines() {}

FrameProcessedPacket::~FrameProcessedPacket() {}

size_t FrameProcessedPacket::read(char *buffer, size_t offset) {
	size_t lines_amount, i;
	
	offset = UartPacket::read(buffer, offset);

	lines_amount = buffer[offset];

	// Every line has 2 bytes: rho and theta
	// TODO check for buffer overflow
	for (i = 1; i <= (lines_amount * 2); i += 2) {
		this->lines.push_back({
			fixed_to_float<1>(buffer[offset + i]),
			fixed_to_float<6>(buffer[offset + i + 1]),
		});
	}

	return offset + i;
}

size_t FrameProcessedPacket::get_length() {
	return 1 + lines.size() + UartPacket::get_length();
}

FrameProcessedPacket::line_vector FrameProcessedPacket::get_lines() {
	return lines;
}

DepartureWarningPacket::DepartureWarningPacket() : UartPacket() {}

DepartureWarningPacket::~DepartureWarningPacket() {}

size_t DepartureWarningPacket::read(char *buffer, size_t offset) {
	offset = UartPacket::read(buffer, offset);

	is_departing = buffer[offset] != 0;

	return offset + 1;
}

size_t DepartureWarningPacket::get_length() {
	return 1 + UartPacket::get_length();
}

bool DepartureWarningPacket::get_is_departing() {
	return is_departing;
}

