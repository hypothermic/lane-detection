/**
 * @file rc_uart_packet.hpp
 * @author Matthijs Bakker
 * @brief Abstraction of the different packet types
 *
 * The UartPacket class and its subclasses provide an abstraction layer
 * over the reading of data messages over the serial port
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

/**
 * A message sent over the UART connection.
 */
class UartPacket {
	public:
		/**
		 * Determins which kind of packet it is.
		 */
		enum class Type {
			STATUS_UPDATE		= (1u << 1),
			FRAME_PROCESSED		= (1u << 2),
			DEPARTURE_WARNING	= (1u << 3),
		};

	private:
		/**
		 * The kind of packet.
		 */
		uint8_t type;

	public:
		UartPacket();
		virtual ~UartPacket();
	
		static size_t read(char *buffer, size_t offset, UartPacket **packet);

		virtual size_t read(char *buffer, size_t offset);
		virtual uint8_t get_type();
		virtual size_t get_length();
};

/**
 * This packet is issued upon start/stop of the video processing loop.
 */
class StatusUpdatePacket : public UartPacket {
	private:
		/**
		 * Whether the video feed is currently getting processed.
		 */
		bool is_processing;

		/**
		 * The HSV segmentation threshold.
		 */
		uint8_t seg_thres;

		/**
		 * The standard deviation (sigma) of the Gaussian filter used
		 * in blurring the image.
		 */
		float	g_sigma;

		/**
		 * The edge detection threshold used in determining whether
		 * pixels resulting from the Sobel filter are valid edges
		 * or not.
		 */
		uint8_t	e_thres;

		/**
		 * The threshold used for detecting peaks in the Hough voting
		 * accumulator.
		 */
		uint8_t h_thres;

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

/**
 * The estimated lane lines are communicated using this packet.
 *
 * It contains the (rho, theta) pairs of all the detected lines.
 */
class FrameProcessedPacket : public UartPacket {
	public:
		using line_vector = std::vector<std::pair<float, float>>;
	private:
		/**
		 * The detected lines described in (rho, theta) format.
		 */
		line_vector lines;

	public:
		FrameProcessedPacket();
		~FrameProcessedPacket();

		size_t read(char *buffer, size_t offset) override;
		size_t get_length() override;

		line_vector get_lines();
};

/**
 * When departure of the road lane is imminent, the lane detector issues this
 * packet with the field <i>is_departure = true</i>.
 *
 * And when the lane departure is over, the packet is sent again but with
 * <i>false</i>.
 */
class DepartureWarningPacket : public UartPacket {
	private:
		/**
		 * Whether the departure alarm is currently active.
		 */
		bool is_departing;

	public:
		DepartureWarningPacket();
		~DepartureWarningPacket();

		size_t read(char *buffer, size_t offset) override;
		size_t get_length() override;

		bool get_is_departing();
};

