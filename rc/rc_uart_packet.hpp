#pragma once

#include <cstdint>
#include <cstddef>

class UartPacket {
	public:
		enum class Type {
			STATUS_UPDATE	= (1u << 1),
			FRAME_PROCESSED = (1u << 2),
		};

	private:
		uint8_t type;

	public:
		UartPacket();
		virtual ~UartPacket();
	
		static size_t read(char *buffer, size_t offset, UartPacket **packet);

		virtual size_t read(char *buffer, size_t offset);
		virtual size_t get_length();
};


class StatusUpdatePacket : public UartPacket {
	private:
		bool is_processing;
		uint8_t seg_thres,
			g_sigma,
			e_thres,
			h_thres;
	public:
		StatusUpdatePacket();
		~StatusUpdatePacket();

		size_t read(char *buffer, size_t offset) override;
		size_t get_length() override;

		uint8_t get_seg_thres();
};

