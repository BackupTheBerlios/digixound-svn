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

#ifndef DATAPIN_HPP
#define DATAPIN_HPP

#include "priority_queue.hpp"
#include "constants.hpp"
#include "message.hpp"
#include "pin.hpp"

namespace uStreamLib {
	/**
	 * This class is a DataPin. A Block uses one or more DataPin
	 * to send/receive data messages from other Blocks. 
	 * A data message transports buffer identifiers that allow
	 * a block to get data chunks on the connecting wire.
	 */
	class US_EXPORT DataPin : public Pin {
	public:
		/* friend classes */
		friend class Block;

		/**
		 * Constructor.
		 */
		DataPin(void);

		/**
		 * Destructor.
		 */
		virtual ~DataPin(void);

		/**
		 * Connect this data pin to another data pin.
		 * @param b the block whose pin we whish to connect to.
		 * @param peer the DataPin to connect this pin to.
		 * @return an integer.
		 */
		int32 connect(Block* b, DataPin* peer)
		{
			return Pin::connect(b, peer);
		}

		/**
		 * Disconnect this data pin from another data pin.
		 * @param b the block whose pin we whish to disconnect from.
		 * @param peer the DataPin to disconnect this pin from.
		 * @return an integer.
		 */
		int32 disconnect(Block* b, DataPin* peer)
		{
			return Pin::disconnect(b, peer);
		}

		/**
		 * Send the specified buffer to all connected peers.
		 * @param buf the buffer to send.
		 * @param pri the priority of this buffer (0 = highest).
		 * @param md avt_metadata to fill correctly by source or filter.
		 * @param di datainfo to fill correctly by source or filter.
		 * @return SUCCESS or FAILURE if peers' queue is full.
		 */
		int32 sendBuffer(DataBuf* buf, int32 pri = 0, avt_metadata* md = NULL,
			datainfo* di = NULL);

		/**
		 * Send the specified buffer to all connected peers.
		 * @param buf the buffer to send.
		 * @param pri the priority of this buffer (0 = highest).
		 * @param md avt_metadata to be filled correctly by source or filter.
		 * @param di datainfo to be filled correctly by source or filter.
		 * @return SUCCESS or FAILURE if peers' queue is full.
		 */
		int32 trySendBuffer(DataBuf* buf, int32 pri = 0,
			avt_metadata* md = NULL, datainfo* di = NULL);

		/**
		 * Send a message to the peers waiting for delivery.
		 * This method blocks until the message is delivered.
		 * @param message a pointer to the message.
		 * @param priority priority of this message.
		 * @return SUCCESS or FAILURE if the pin is unconnected.
		 */
		int32 sendMessage(dmessage* m, int32 priority = 0);

		/**
		 * Send a message to the peer.
		 * This method returns FAILURE if the message cannot be delivered.
		 * It is non blocking.
		 * @param message a pointer to the message.
		 * @return SUCCESS or FAILURE if the message cannot be delivered now.
		 * SUCCESS means that at least one message was sent.
		 */
		int32 trySendMessage(dmessage* m, int32 priority = 0);

		/**
		 * Receive a message from the peer.
		 * This method blocks until a message is received.
		 * @param message structore to store the message into.
		 * @return SUCCESS or FAILURE if the pin is unconnected.
		 */
		int32 recvMessage(dmessage* m);

		/**
		 * Receive a message from the peer.
		 * This method checks if there is a message in the queue.
		 * It is non blocking.
		 * @param message structore to store the message into.
		 * @return SUCCESS or FAILURE if no message can be received now.
		 */
		int32 tryRecvMessage(dmessage* m);
	
	protected:
		/**
		 * Build a DataPin. The accepted parameters must be set
		 * using some tuning criteria.
		 * @param b block that owns this pin.
		 * @param name descriptive name of this data pin.
		 * @param dir pin direction, one of DIR_INPUT or DIR_OUTPUT.
		 * @param bufsz preferred size of a managed buffer.
		 * @param bufcount preferred count of managed buffers.
		 * @param queuesz size of the data queues.
		 */
		int32 init(Block* b, // block this pin belongs to
	  		char* name, // descriptive name
			uint8 dir, // pin direction
			uint32 bufsz, // preferred buffer size
			uint32 bufcount, // preferred count of buffers
			int32 queuesz 	// queue size
			);
	private:
		/*
		 * Other constructors protected.
		 */
		DataPin(DataPin&)
		{
		}

		/* input priority queue used to receive messages */
		SharedQueue _iq;
	};
}

#endif
