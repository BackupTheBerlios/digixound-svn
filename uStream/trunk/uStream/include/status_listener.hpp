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

#ifndef STATUS_LISTENER_HPP
#define STATUS_LISTENER_HPP

#include "block_handler.hpp"
#include "message.hpp"
#include "control_pin.hpp"
#include "data_pin.hpp"

namespace uStreamLib {
	/**
	 * This class is a status listener. Its main task is to receive status
	 * messages from a block. When a block receives and executes a control
	 * message, it may notify message execution results to status listeners
	 * if the entity who sent the message provided a status listener pointer
	 * into the appropriate field of the cmessage structure.
	 */
	class US_EXPORT StatusListener : public Object {
	public:
		/**
		 * Constructor.
		 * Build a StatusListener for a specific block.
		 * @param block the block associated to this status listener.
		 */
		StatusListener(Block* b);

		/**
		 * Destructor.
		 */
		virtual ~StatusListener(void);

		/**
		 * Method used by a block for status message delivery.
		 * Override this method if you need to provide your own
		 * status listener.
		 * @param sm pointer to the status message.
		 * @return SUCCESS or FAILURE if status message cannot be
		 * delivered.
		 */
		virtual int32 notifyStatusMessage(smessage*)
		{
			return SUCCESS;
		}

		/**
		 * This method returns the block whose messages this status listener
		 * intercepts.
		 */
		Block* getBlock(void)
		{
			return _block;
		}
	private:
		/* copy constructor not available */
		StatusListener(StatusListener&)
			: Object(UOSUTIL_RTTI_STATUS_LISTENER)
		{
		}

		/* Block this listener refers to */
		Block* _block;
	};
}

#endif
