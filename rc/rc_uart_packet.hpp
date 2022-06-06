/**
 * @file rc_uart_packet.hpp
 * @author Matthijs Bakker
 * @brief Abstraction of the different packet types
 *
 * The UartPacket class and its subclasses provide an abstraction layer
 * over the reading of data messages over the serial port
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * A message sent over the UART connection.
 *
 * The type and subclass indicate which kind of packet it is
 */
class UartPacket {
	public:
		/**
		 * Determins which kind of packet it is
		 */
		enum class Type {
			STATUS_UPDATE		= (1u << 1),
			FRAME_PROCESSED		= (1u << 2),
			DEPARTURE_WARNING	= (1u << 3),
		};

	private:
		/**
		 * The kind of packet it is. This can also be
		 * determined by its subclass
		 */
		uint8_t type;

	public:
		/**
		 * Construct a new UartPacket
		 */
		UartPacket();

		/**
		 * Release this packet's internal resources
		 */
		virtual ~UartPacket();
	
		/**
		 * Try to reconstruct a packet from a buffer.
		 * Reading will start at the specified <i>offset</i>
		 * A reference to the reconstructed packet will
		 * be assigned to the <i>packet</i> pointer.
		 *
		 * If no packet is read, the address of the
		 * pointer will not be changed, indicating failure
		 *
		 * @param buffer	The buffer to read from
		 * @param offset	Where to start reading
		 * @param packet	Return value location
		 *
		 * @return		Amount of bytes that
		 * 			have been read
		 */
		static size_t read(char *buffer, size_t offset, UartPacket **packet);

		/**
		 * Reconstruct the packet data from a buffer
		 *
		 * @param buffer	The buffer to read from
		 * @param offset	Where to start reading
		 *
		 * @return		How many bytes have been read
		 */
		virtual size_t read(char *buffer, size_t offset);

		/**
		 * Get the type of the packet
		 *
		 * @return		Packet type as 8-bit int
		 */
		virtual uint8_t get_type();

		/**
		 * Get the amount of bytes the packet contains
		 *
		 * @return		Size of the packet
		 */
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
		/*
		 * @inheritDoc
		 */
		StatusUpdatePacket();
		
		/*
		 * @inheritDoc
		 */
		~StatusUpdatePacket();

		/*
		 * @inheritDoc
		 */
		size_t read(char *buffer, size_t offset) override;
		
		/*
		 * @inheritDoc
		 */
		size_t get_length() override;

		/*
		 * @inheritDoc
		 */
		bool get_is_processing();

		/*
		 * @inheritDoc
		 */
		uint8_t get_seg_thres();

		/*
		 * @inheritDoc
		 */
		float get_g_sigma();

		/*
		 * @inheritDoc
		 */
		uint8_t get_e_thres();

		/*
		 * @inheritDoc
		 */
		uint8_t get_h_thres();
};

/**
 * The estimated lane lines are communicated using this packet.
 *
 * It contains the (rho, theta) pairs of all the detected lines.
 */
class FrameProcessedPacket : public UartPacket {
	public:
		/**
		 * Type for holding a vector with rho,theta line parameters
		 */
		using line_vector = std::vector<std::pair<float, float>>;

	private:
		/**
		 * The detected lines described in (rho, theta) format.
		 */
		line_vector lines;

	public:
		/*
		 * @inheritDoc
		 */
		FrameProcessedPacket();

		/*
		 * @inheritDoc
		 */
		~FrameProcessedPacket();

		/*
		 * @inheritDoc
		 */
		size_t read(char *buffer, size_t offset) override;

		/*
		 * @inheritDoc
		 */
		size_t get_length() override;

		/*
		 * @inheritDoc
		 */
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
		 * Whether the departure alarm is currently active or not
		 */
		bool is_departing;

	public:
		/*
		 * @inheritDoc
		 */
		DepartureWarningPacket();

		/*
		 * @inheritDoc
		 */
		~DepartureWarningPacket();

		/*
		 * @inheritDoc
		 */
		size_t read(char *buffer, size_t offset) override;

		/*
		 * @inheritDoc
		 */
		size_t get_length() override;

		/*
		 * @inheritDoc
		 */
		bool get_is_departing();
};

