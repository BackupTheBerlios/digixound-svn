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

#ifndef CONTROLPIN_HPP
#define CONTROLPIN_HPP

#include "priority_queue.hpp"
#include "constants.hpp"
#include "message.hpp"
#include "pin.hpp"

namespace uStreamLib {
	/**
	 * This class is a ControlPin. A Block uses a ControlPin
	 * to get control messages from other Blocks. A ControlPin
	 * can be used to transmit/receive control messages (of type
	 * message - which is a struct -) and chunks of data using
	 * databuffers exchanged by mean of a buffer pool.
	 * A ControlPin contains one priority queue used to exchange
	 * control messages. This class is a bidirectional communication
	 * channel between two Blocks. Remember that you need a Wire to
	 * connect two Pins. A Wire is used to exchange control data
	 * chunks (not control messages) between blocks.
	 */
	class US_EXPORT ControlPin : public Pin {
	public:

		/**
		 * Constructor.
		 */
		ControlPin(void);

		/**
		  * Destructor.
		  */
		virtual ~ControlPin(void);

		/**
		 * Build a ControlPin. The accepted parameters must be set
		 * using some tuning criteria.
		 * @param b block this pin belongs to.
		 * @param bufsz preferred size of a managed buffer.
		 * @param bufcount preferred count of managed buffers.
		 * @param queuesz size of the control queues.
		 */
		int32 init(Block* b, // parent block
	  		uint32 bufsz, // preferred buffer size
	  		uint32 bufcount, // preferred count of buffers
	  		int32 queuesz 	// queue size
			);

		/**
		 * Connect this control pin to another control pin.
		 * @param b the block whose pin we whish to connect to.
		 * @param peer the ControlPin to connect this pin to.
		 * @return an integer.
		 */
		int32 connect(Block* b, ControlPin* peer)
		{
			return Pin::connect(b, peer);
		}

		/**
		 * Disconnect this control pin from another control pin.
		 * @param b the block whose pin we whish to disconnect from.
		 * @param peer the ControlPin to disconnect this pin from.
		 * @return an integer.
		 */
		int32 disconnect(Block* b, ControlPin* peer)
		{
			return Pin::disconnect(b, peer);
		}

		/**
		 * Send a message to the peers waiting for delivery.
		 * This method blocks until the message is delivered.
		 * @param message a pointer to the message.
		 * @param priority priority of this message.
		 * @return SUCCESS or FAILURE if the pin is unconnected.
		 */
		int32 sendMessage(cmessage* m, int32 priority = 0);

		/**
		 * Send a message to the peers.
		 * This method returns FAILURE if the message cannot be delivered
		 * to all recipients. It is non blocking.
		 * @param message a pointer to the message.
		 * @return SUCCESS or FAILURE if the message cannot be delivered now.
		 * SUCCESS means that at least one message was sent.
		 */
		int32 trySendMessage(cmessage* m, int32 priority = 0);

		/**
		 * Receive a message from the peer.
		 * This method blocks until a message is received.
		 * @param message structore to store the message into.
		 * @return SUCCESS or FAILURE if the pin is unconnected.
		 */
		int32 recvMessage(cmessage* m);

		/**
		 * Receive a message from the peer.
		 * This method checks if there is a message in the queue.
		 * It is non blocking.
		 * @param message structore to store the message into.
		 * @return SUCCESS or FAILURE if no message can be received now.
		 */
		int32 tryRecvMessage(cmessage* m);

		/**
		 * Put a message into the input queue for this control pin.
		 * This method blocks untils the message is put.
		 * @param m control message to put into the queue.
		 * @param priority priority of the message >= 0.
		 * @return SUCCESS or FAILURE;
		 */
		int32 putMessage(cmessage* m, int32 priority = 0)
		{
			m->serial = _cmserial++;
			return _iq.put((char *) m, sizeof(cmessage), priority);
		}

		/**
		 * Put a message into the input queue for this control pin.
		 * If the message cannot be rapidly put, the method returns.
		 * This method is unblocking.
		 * @param m message to put.
		 * @param priority priority of the message >= 0.
		 * @return SUCCESS or FAILURE if message cannot be put.
		 */
		int32 tryPutMessage(cmessage* m, int32 priority = 0)
		{
			m->serial = _cmserial++;
			return _iq.tryPut((char *) m, sizeof(cmessage), priority);
		}

		/**
		 * Get a message from the input queue for this control pin.
		 * This method blocks untils the message is get. This method
		 * does not check if the pin is connected.
		 * @param m control message to put into the queue.
		 * @return SUCCESS or FAILURE;
		 */
		int32 getMessage(cmessage* m)
		{
			return _iq.get((char *) m, sizeof(cmessage));
		}

		/**
		 * Get a message from the input queue for this control pin.
		 * If the message cannot be rapidly put, the method returns.
		 * This method is unblocking. This method does not check if
		 * the pin is connected.
		 * @param m message to put.
		 * @return SUCCESS or FAILURE if message cannot be put.
		 */
		int32 tryGetMessage(cmessage* m)
		{
			return _iq.tryGet((char *) m, sizeof(cmessage));
		}
	private:
		/*
		 * Other constructors protected.
		 */
		ControlPin(ControlPin&)
		{
		}

		/**
		 * This initialization method is reserved. It can be invoked only
		 * by BlockAdapters.
		 */
		int32 init(char* name, // pin's name
	  		uint32 bufsz, // preferred buffer size
	  		uint32 bufcount, // preferred count of buffers
	  		int32 queuesz 	// queue size
			);

		/* input priority queue used to receive control messages */
		PriorityQueue _iq;

		/* control message serial counter */
		uint32 _cmserial;
	};
}

#endif
