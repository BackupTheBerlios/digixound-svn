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

#include "control_pin.hpp"

namespace uStreamLib {
	ControlPin::ControlPin(void)
		: _cmserial(0)
	{
		// nothing to do
	}

	ControlPin::~ControlPin(void)
	{
		// nothing to do
	}

	int32 ControlPin::init(Block* b, // parent block
	uint32 bufsz, // preferred buffer size
	uint32 bufcount, // preferred count of buffers
	int32 queuesz     // queue size
	)
	{
		int32 ret = 0;

		// initialize parent
		ret = Pin::init(b, "ControlPin", DIR_IO, bufsz, bufcount);
		if (ret == FAILURE)
			return FAILURE;

		// initialize queue
		ret = _iq.init(getAbsoluteName(), sizeof(cmessage), queuesz,
					US_PRIORITY_LEVELS, US_OOB_QUEUE_SIZE);
		if (ret == FAILURE)
			return FAILURE;

		// setup data type    
		setDataType(DT_BYTES);

		// ok
		return SUCCESS;
	}

	int32 ControlPin::init(char* name, // pin's name
	uint32 bufsz, // preferred buffer size
	uint32 bufcount, // preferred count of buffers
	int32 queuesz     // queue size
	)
	{
		int32 ret = 0;

		// initialize parent
		ret = Pin::init(NULL, name, DIR_IO, bufsz, bufcount);
		if (ret == FAILURE)
			return FAILURE;

		// initialize queue
		ret = _iq.init(getAbsoluteName(), sizeof(cmessage), queuesz,
					US_PRIORITY_LEVELS, US_OOB_QUEUE_SIZE);
		if (ret == FAILURE)
			return FAILURE;

		// setup data type
		setDataType(DT_BYTES);

		// ok
		return SUCCESS;
	}

	int32 ControlPin::sendMessage(cmessage* m, int32 priority)
	{
		Enumeration* en = NULL;
		int32 ret = 0;

		if (getStatus() == Pin::UNCONNECTED) {
			// DEBUG 
			UOSUTIL_DOUT(("unconnected")); 
			return FAILURE;
		}

		m->serial = _cmserial++;

		en = getPeers();
		while (en->hasMoreElements()) {
			ControlPin* p = (ControlPin*) en->nextElement();

			// DEBUG
			UOSUTIL_DOUT(("ControlPin::sendMessage(): -> %s\n",
				p->getAbsoluteName()));

			ret = p->_iq.put((char *) m, sizeof(cmessage), priority);
			if (ret == FAILURE)
				return FAILURE;
		}

		return SUCCESS;
	}

	int32 ControlPin::trySendMessage(cmessage* m, int32 priority)
	{
		Enumeration* en = NULL;
		int32 ret = 0;

		if (getStatus() == Pin::UNCONNECTED) {
			// DEBUG
			UOSUTIL_DOUT(("unconnected")); 
			return FAILURE;
		}

		m->serial = _cmserial++;

		en = getPeers();
		while (en->hasMoreElements()) {
			ControlPin* p = (ControlPin*) en->nextElement();

			// DEBUG
			UOSUTIL_DOUT(("ControlPin::trySendMessage(): -> %s\n",
				p->getAbsoluteName()));

			ret = p->_iq.tryPut((char *) m, sizeof(cmessage), priority);
			if (ret == FAILURE) {
				/* to replace with logging methods */
				/*
						  printf
							 (
							  "ControlPin::trySendMessage(): failed for %s\n",
							  p->getAbsoluteName()
							  );
						  */
				// DEBUG
				UOSUTIL_DOUT(("ControlPin::trySendMessage(): failed for %s\n",
					p->getAbsoluteName()));
			}
		}

		return ret;
	}

	int32 ControlPin::recvMessage(cmessage* m)
	{
		if (getStatus() == Pin::UNCONNECTED) {
			// DEBUG
			UOSUTIL_DOUT(("unconnected")); 
			return FAILURE;
		}

		return _iq.get((char *) m, sizeof(cmessage));
	}

	int32 ControlPin::tryRecvMessage(cmessage* m)
	{
		if (getStatus() == Pin::UNCONNECTED) {
			// DEBUG
			UOSUTIL_DOUT(("unconnected")); 
			return FAILURE;
		}

		return _iq.tryGet((char *) m, sizeof(cmessage));
	}
}
