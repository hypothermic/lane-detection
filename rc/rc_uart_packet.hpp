#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

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
		virtual uint8_t get_type();
		virtual size_t get_length();
};

class StatusUpdatePacket : public UartPacket {
	private:
		bool is_processing;
		uint8_t seg_thres;
		float	g_sigma;
		uint8_t	e_thres,
			h_thres;
	public:
		StatusUpdatePacket();
		~StatusUpdatePacket();

		size_t read(char *buffer, size_t offset) override;
		size_t get_length() override;

		bool get_is_processing();
		uint8_t get_seg_thres();
		float get_g_sigma();
		uint8_t get_e_thres();
		uint8_t get_h_thres();
};

class FrameProcessedPacket : public UartPacket {
	public:
		using line_vector = std::vector<std::pair<float, float>>;
	private:
		line_vector lines;

	public:
		FrameProcessedPacket();
		~FrameProcessedPacket();

		size_t read(char *buffer, size_t offset) override;
		size_t get_length() override;

		line_vector get_lines();
};

