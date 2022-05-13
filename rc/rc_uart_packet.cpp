#include "rc_uart_packet.hpp"

#include <type_traits>

#include "rc_log.hpp"

UartPacket::UartPacket() {}

UartPacket::~UartPacket() {}

size_t UartPacket::read(char *buffer, size_t offset, UartPacket **packet) {
	UartPacket *result;

	switch (buffer[offset]) {
		case static_cast<uint8_t>(UartPacket::Type::STATUS_UPDATE):
			result = new StatusUpdatePacket();
			break;
		default:
			rc_log_error("No packet type");
			return offset + 1;
	}

	offset = result->read(buffer, offset);

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

	is_processing = buffer[offset] != 0;
	seg_thres = buffer[offset + 1];
	g_sigma = buffer[offset + 2];
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

uint8_t StatusUpdatePacket::get_g_sigma() {
	return this->g_sigma;
}

uint8_t StatusUpdatePacket::get_e_thres() {
	return this->e_thres;
}

uint8_t StatusUpdatePacket::get_h_thres() {
	return this->h_thres;
}

