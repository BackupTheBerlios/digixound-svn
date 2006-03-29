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

#include "block.hpp"

namespace uStreamLib {
	Pin::Pin(void)
		: _block(NULL), _status(UNCONNECTED), _direction(DIR_IO),
		_bpSet(false), _pref_bufsz(0), _pref_bufcount(0), _real_bufsz(0),
		_real_bufcount(0), _ibp(NULL), _dt(DT_UNDEF)
	{
		// nothing to do
	}

	Pin::~Pin(void)
	{
		MutexLocker ml(this);
	}

	int32 Pin::init(Block* b, // parent block for this pin
	char* name, // descriptive name
	uint8 dir, // pin direction
	uint32 bufsz, // preferred buffer size
	uint32 bufcount   // preferred count of buffers
	)
	{
		char* bname = NULL, tmp[4096];
		int32 ret = 0;

		// initialize parent
		ret = Mutex::init();
		if (ret == FAILURE)
			return FAILURE;

		ret = ConfigTable::init(US_CONFIGTABLE_HSIZE);
		if (ret == FAILURE)
			return FAILURE;

		// check pin direction
		if (dir != DIR_INPUT && dir != DIR_OUTPUT && dir != DIR_IO)
			return FAILURE;

		// initialize buffers
		ret = _name.init(name);
		if (ret == FAILURE)
			return FAILURE;

		ret = _peers.init(33);
		if (ret == FAILURE)
			return FAILURE;

		ret = _wires.init(33);
		if (ret == FAILURE)
			return FAILURE;

		// initialize members
		_block = b;
		_status = UNCONNECTED;
		_direction = dir;
		_bpSet = false;
		_pref_bufsz = bufsz;
		_pref_bufcount = bufcount;
		_real_bufsz = bufsz;
		_real_bufcount = bufcount;
		_ibp = NULL;
		_dt = DT_UNDEF;

		memset(&_subtype, 0, sizeof(SubType));

		if (b)
			bname = b->getName();
		else
			bname = "NoBlock";

		snprintf(tmp, sizeof(tmp), "%s.%s", bname, name);

		ret = _absname.init(tmp);
		if (ret == FAILURE)
			return FAILURE;

		return SUCCESS;
	}

	void Pin::lockTable(int32 what)
	{
		switch (what) {
		case PEERS_TABLE:
			_peers.lock(); break;
		case WIRES_TABLE:
			_wires.lock(); break;
		default:
			_peers.lock();
			_wires.lock();
		}
	}

	void Pin::unlockTable(int32 what)
	{
		switch (what) {
		case PEERS_TABLE:
			_peers.unlock(); break;
		case WIRES_TABLE:
			_wires.unlock(); break;
		default:
			_peers.unlock();
			_wires.unlock();
		}
	}

	int32 Pin::connect(Block* b, Pin* peer)
	{
		int32 ret = 0;

		if (b && !b->belongs(peer)) {
			printf("%s: I don't belong to block %s\n",
				peer->getAbsoluteName(), b ? b->getName() : "NoBlock");
			return FAILURE;
		}

		// DEBUG
		UOSUTIL_DOUT(("Adding pin %s to pin %s peers table\n",
			peer->getAbsoluteName(), getAbsoluteName()));

		ret = _peers.pput(peer->getAbsoluteName(), (char *) peer);
		if (ret != SUCCESS)
			return FAILURE;

		// DEBUG
		UOSUTIL_DOUT(("Pin(%s): connected to %s\n", getAbsoluteName(),
			peer->getAbsoluteName()));

		_status = CONNECTED; return SUCCESS;
	}

	int32 Pin::disconnect(Block* b, Pin* peer)
	{
		int32 ret = 0;

		if (b && !b->belongs(peer)) {
			printf("%s: I don't belong to block %s\n",
				peer->getAbsoluteName(), b ? b->getName() : "NoBlock");

			return FAILURE;
		}

		ret = _peers.del(peer->getAbsoluteName());
		if (ret != SUCCESS) {
			puts("cannot remove peer"); return FAILURE;
		}

		ret = _wires.del(peer->getAbsoluteName());
		if (ret != SUCCESS) {
			puts("cannot remove wire"); return FAILURE;
		}

		// DEBUG
		UOSUTIL_DOUT(("Pin(%s): disconnected from %s\n", getAbsoluteName(),
			peer->getAbsoluteName()));

		if (!getPeersCount())
			_status = UNCONNECTED;

		return SUCCESS;
	}

	char* Pin::getDataTypeString(DataType dt)
	{
		switch (dt) {
		case DT_UNDEF:
			return "DT_UNDEF";
		case DT_AUDIO:
			return "DT_AUDIO";
		case DT_VIDEO:
			return "DT_VIDEO";
		case DT_TEXT:
			return "DT_TEXT";
		case DT_BYTES:
			return "DT_BYTES";
		}

		return "UNDEFINED";
	}
}
