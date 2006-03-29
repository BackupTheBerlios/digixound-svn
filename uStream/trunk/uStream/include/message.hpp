/*
  uSTREAM LIGHT-WEIGHT STREAMING ARCHITECTURE
  Copyright (C) 2005 Luis Serrano (luis@kontrol-dj.com) 
  
  Based on DANUBIO STREAMING ARCHITECTURE by Michele Iacobellis (m.iacobellis@nexotech.it)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "constants.hpp"
#include "avt_metadata.hpp"

/* status message buffer size */
#define SM_MESSAGE_SZ 128

namespace uStreamLib {
	/* Declare class Block and Pin (for sender/receiver) */
	class StatusListener;
	class Block;
	class Pin;

	/**
	 * The Control Message.
	 * This structure is the message that each block 
	 * sends/receives to/from other blocks for controlling
	 * all behaviours. This is a control message and is 
	 * transmitted only on control pins.
	 */
	struct US_EXPORT cmessage_t {
		/** message code */
		uint32 code;

		/** seek position info */
		seekinfo seek;

		/** carried buffer id */
		uint32 bid;

		/** message serial number */
		uint32 serial;

		/** opaque user data pointer */
		void* userdata;

		/** status listener for response on this message */
		StatusListener* status_listener;

		/** sender block of this message */
		Block* from;
	};

	typedef struct cmessage_t cmessage;

	/**
	 * The Data Message.
	 * This structure is the message that each block 
	 * sends/receives to/from the connected block.
	 * This is a data message and is used to carry
	 * data information only.
	 */
	struct US_EXPORT dmessage_t {
		/** carried buffer id */
		uint32 bid;

		/** audio/video/text data description */
		avt_metadata info;

		/** data description for control */
		datainfo di;

		/** sender block of this message */
		Block* from;

		/** sender pin of this message */
		Pin* from_pin;
	};

	typedef struct dmessage_t dmessage;

	/**
	 * This structure is the status message.
	 * A status message is returned by a block when
	 * the block needs to notify status after control
	 * message execution.
	 */
	struct US_EXPORT smessage_t {
		/** control message code */
		uint32 code;

		/** serial number of control message */
		uint32 serial;

		/** event handler return value */
		int32 eh_return;

		/** current block status */
		int32 status;

		/** error string message buffer */
		char error[SM_MESSAGE_SZ];

		/** flag: there is a buffer in the pool */
		bool buffer_ready;

		/** buffer id (if buffer_ready is true) */
		uint32 bid;
	};

	typedef struct smessage_t smessage;
}

#endif
