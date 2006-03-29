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

#ifndef DEFAULT_STATUS_LISTENER_HPP
#define DEFAULT_STATUS_LISTENER_HPP

#include "status_listener.hpp"

namespace uStreamLib {
	/**
	 * This class provides a default status listener. It is useful for
	 * block status response interception so that control messages
	 * execution can be synchronous.
	 */
	class US_EXPORT DefaultStatusListener : public StatusListener {
	public:
		/**
		 * Constructor. Build a default status listener.
		 * @param block the block this status listener is associated to.
		 * @param queuesz count of status messages to store into the internal
		 * queue.
		 */
		DefaultStatusListener(Block* b, uint32 queuesz = 10);

		/**
		 * Destructor.
		 */
		virtual ~DefaultStatusListener(void);

		/**
		 * Notification method. This has been overridden by this class
		 * so that status messages are stored into an internal queue.
		 */
		int32 notifyStatusMessage(smessage* sm)
		{
			return _smq.put((char *) sm, sizeof(smessage));
		}

		/**
		 * This method allows to get a status message after having sent
		 * a control message to a block.
		 * @param sm a pointer to an smessage structure (output parameter).
		 * @param wait true means wait until a message is present (if wait is
		 * true, this method blocks, otherwise it is non blocking).
		 * @return SUCCESS or FAILURE if no status messages are present in the
		 * queue.
		 */
		int32 getStatusMessage(smessage* sm, bool wait)
		{
			int32 ret = 0;

			if (wait)
				ret = _smq.get((char *) sm, sizeof(smessage));
			else
				ret = _smq.tryGet((char *) sm, sizeof(smessage));

			return ret;
		}
	private:
		/* copy constructor not available */
		DefaultStatusListener(DefaultStatusListener&)
			: StatusListener(NULL)
		{
		}

		/* status message queue */
		SharedQueue _smq;
	};
}

#endif
