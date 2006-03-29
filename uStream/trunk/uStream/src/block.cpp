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

#include <ctype.h>

#include "block.hpp"
#include "block_manager.hpp"

namespace uStreamLib {
	BlockHandler::BlockHandler(Block* b, char* name)
	{
		Logger* l = b->getBlockManager()->getLogger();
		int32 ret = 0;

		ret = CallBack::init(name);
		if (ret == FAILURE) {
			if (l)
				l->log(Logger::LEVEL_ERROR,
					"%s: Callback initialization failed (%s)", b->getName(),
					name);
		}

		_block = b;

		if (l)
			l->log(Logger::LEVEL_NOTICE, "%s: Callback %s initialized.",
				b->getName(), name);
	}

	BlockHandler::~BlockHandler(void)
	{
		// nothing to do
	}

	StatusListener::StatusListener(Block* b)
		: Object(UOSUTIL_RTTI_STATUS_LISTENER), _block(b)
	{
		// nothing to do
	}

	StatusListener::~StatusListener(void)
	{
		// nothing to do
	}

	DefaultStatusListener::DefaultStatusListener(Block* b, uint32 queuesz)
		: StatusListener(b)
	{
		Logger* l = b->getBlockManager()->getLogger();

		char tmp[4096];
		int32 ret = 0;

		snprintf(tmp, sizeof(tmp), "%s[SL]", b->getName());

		ret = _smq.init(tmp, sizeof(smessage), queuesz);
		if (ret == FAILURE) {
			if (l)
				l->log(Logger::LEVEL_ERROR,
					"%s: Status listener initialization failed.", b->getName());
		}

		if (l)
			l->log(Logger::LEVEL_DEBUG, "%s: Status listener initialized.",
				b->getName());
	}

	DefaultStatusListener::~DefaultStatusListener(void)
	{
		// nothing to do
	}

	char* Block::_cmdstrings[Block::EVENT_HANDLERS_TABLE_SIZE];

	Block::Block(void)
		: _bm(NULL)
	{
		// nothing to do
	}

	Block::~Block(void)
	{
		// DEBUG
		UOSUTIL_DOUT(("~Block(): entered\n"));

		// destroy callbacks
		destroyHandlers();

		// destroy pins
		destroyPins();

		// delete wires for control pins
		Enumeration* ew = _cp.getWires();
		while (ew->hasMoreElements()) {
			Wire* w = (Wire*) ew->nextElement();
			if (w)
				delete w;
		}

		// DEBUG
		UOSUTIL_DOUT(("~Block(): exited\n"));
	}

	int32 Block::init(BlockManager* bm, char* name, uint32 bufsz,
		uint32 bufcount, int32 queuesz)
	{
		int32 ret = 0, i = 0;

		// initialize members
		_bm = bm;

		// initialize thread
		ret = Thread::init(name);
		if (ret == FAILURE)
			return FAILURE;

		// initialize config table
		ret = ConfigTable::init(US_CONFIGTABLE_HSIZE);
		if (ret == FAILURE)
			return FAILURE;

		// initialize shared variables
		ret = _status.init(STATUS_READY);
		if (ret == FAILURE)
			return FAILURE;

		ret = _info.init("");
		if (ret == FAILURE)
			return FAILURE;

		ret = _errorstring.init("");
		if (ret == FAILURE)
			return FAILURE;

		// create control pin
		ret = _cp.init(this, bufsz, bufcount, queuesz);
		if (ret == FAILURE)
			return FAILURE;

		// create table for input data pins
		ret = _idp.init(US_DPT_HSIZE);
		if (ret == FAILURE)
			return FAILURE;

		// create table for output data pins
		ret = _odp.init(US_DPT_HSIZE);
		if (ret == FAILURE)
			return FAILURE;

		// create toString buffer
		ret = _tostring.init(255);
		if (ret == FAILURE)
			return FAILURE;

		_tostring.set(0);

		// initialize handlers tables
		for (i = 0; i < EVENT_HANDLERS_TABLE_SIZE; i++)
			_etable[i] = NULL;

		for (i = 0; i < ACTION_HANDLERS_TABLE_SIZE; i++)
			_atable[i] = NULL;

		// create table mutex
		ret = _mutexTable.init();
		if (ret == FAILURE)
			return FAILURE;

		// ok
		return SUCCESS;
	}

	DataPin* Block::createDataPin(char* name, uint8 dir, uint32 bufsz,
		uint32 bufco, int32 queuesz)
	{
		DataPin* dp = NULL;
		char tmp[4096];
		int32 ret = 0;

		dp = new DataPin();
		if (!dp)
			return NULL;

		ret = dp->init(this,		  // pointer to this block
		 name,  		// pin's name
		 dir,   		// direction (one of INPUT or OUTPUT)
		 bufsz, 		// preferred buffer size
		 bufco, 		// preferred buffers count
		 queuesz		// pin's input queue size (no. of elements)
		);
		if (ret == FAILURE) {
			delete dp; return NULL;
		}

		switch (dir) {
		case Pin::DIR_INPUT:
			ret = _idp.pput(dp->getName(), (char *) dp);
			if (ret != SUCCESS) {
				snprintf(tmp, sizeof(tmp),
					"Cannot add input pin \"%s\" to pins table (ret = %d)",
					name, ret);
				setErrorString(tmp);

				// free resources
				delete dp;

				// fail
				return NULL;
			}
			break;
		case Pin::DIR_OUTPUT:
			ret = _odp.pput(dp->getName(), (char *) dp);
			if (ret != SUCCESS) {
				snprintf(tmp, sizeof(tmp),
					"Cannot add output pin \"%s\" to pins table (ret = %d)",
					name, ret);
				setErrorString(tmp);

				// free resources 
				delete dp;

				// fail
				return NULL;
			}
			break;
		case Pin::DIR_IO:
			setErrorString("Data pin cannot be bidirectional");

			// free resources
			delete dp;

			// fail 
			return NULL;
		}

		// ok
		return dp;
	}

	char* Block::toString(void)
	{
		char tmp[4096];

		snprintf(tmp, sizeof(tmp),
			"BLOCK <%s>\n"
			" Info       : %s\n"
			" Input Pins : %d\n"
			" Output Pins: %d\n"
			" Status     : %s\n",
			getName(), getInfo(), _idp.getCount(), _odp.getCount(),
			getStatusString(getStatus()));

		_tostring.copy(tmp);
		return _tostring.toString();
	}

	int32 Block::executeEventHandler(int32 event_id)
	{
		MutexLocker ml(&_mutexTable);

		if (event_id < 0 || event_id >= EVENT_HANDLERS_TABLE_SIZE) {
			return HANDLER_UNDEFINED;
		}

		if (!_etable[event_id]) {
			return HANDLER_UNDEFINED;
		}

		_etable[event_id]->_code = event_id;
		return _etable[event_id]->perform();
	}

	int32 Block::executeActionHandler(int32 action_id)
	{
		MutexLocker ml(&_mutexTable);

		if (action_id < 0 || action_id >= ACTION_HANDLERS_TABLE_SIZE) {
			return HANDLER_UNDEFINED;
		}

		if (!_atable[action_id]) {
			return HANDLER_UNDEFINED;
		}

		_atable[action_id]->_code = action_id;
		return _atable[action_id]->perform();
	}

	char** Block::getCommandStrings(uint32* cmd_count)
	{
		*cmd_count = EVENT_HANDLERS_TABLE_SIZE;

		/*
		  * Array index must be the same as the EVENT_* enum !
		  */

		_cmdstrings[0] = "reset";
		_cmdstrings[1] = "start";
		_cmdstrings[2] = "stop";
		_cmdstrings[3] = "pause";
		_cmdstrings[4] = "bufferize";
		_cmdstrings[5] = "seek";
		_cmdstrings[6] = "tell";
		_cmdstrings[7] = "skip";
		_cmdstrings[8] = "quit";
		_cmdstrings[9] = "data ready";
		_cmdstrings[10] = "timeout";
		_cmdstrings[11] = "terminate";
		_cmdstrings[12] = "command";  

		_cmdstrings[13] = "user 1";
		_cmdstrings[14] = "user 2";
		_cmdstrings[15] = "user 3";
		_cmdstrings[16] = "user 4";
		_cmdstrings[17] = "user 5";
		_cmdstrings[18] = "user 6";
		_cmdstrings[19] = "user 7";

		return _cmdstrings;
	}

	char* Block::getStatusString(int32 status)
	{
		switch (status) {
		case STATUS_READY:
			return "ready";
		case STATUS_STARTED:
			return "started";
		case STATUS_STOPPED:
			return "stopped";
		case STATUS_PAUSED:
			return "paused";
		case STATUS_SEEKING:
			return "seeking";
		case STATUS_SKIPPING:
			return "skipping";
		case STATUS_BUFFERIZING:
			return "bufferizing";
		case STATUS_RESETTING:
			return "resetting";
		case STATUS_QUITTING:
			return "quitting";
		case STATUS_TERMINATING:
			return "terminating";
		case STATUS_TELLING:
			return "telling";
		case STATUS_ERROR:
			return "error";
		}

		return "UNDEFINED";
	}

	char* Block::getTypeString(int32 type_id)
	{
		switch (type_id) {
		case TYPE_SOURCE:
			return "SOURCE";
		case TYPE_FILTER:
			return "FILTER";
		case TYPE_SINK:
			return "SINK";
		}

		return "UNDEFINED";
	}

	bool Block::belongs(Pin* pin)
	{
		Pin* p = NULL;

		// check control pin
		if (&_cp == pin)
			return true;

		// check input pins
		p = (Pin *) _idp.get(pin->getName());
		if (p)
			return true;

		// check output pins
		p = (Pin *) _odp.get(pin->getName());
		if (p)
			return true;

		// sorry, not belonging
		return false;
	}

	void Block::destroyPins(void)
	{
		Enumeration* en = NULL;

		// lock input pins table
		_idp.lock();

		// delete input pins
		en = _idp.values();
		while (en->hasMoreElements())
			delete (DataPin *) en->nextElement();

		// unlock input pins table
		_idp.unlock();

		// lock output pins table
		_odp.lock();

		// delete output pins
		en = _odp.values();
		while (en->hasMoreElements())
			delete (DataPin *) en->nextElement();

		// unlock output pins table
		_odp.unlock();
	}

	void Block::destroyHandlers(void)
	{
		int32 i = 0;

		MutexLocker ml(&_mutexTable);

		for (i = 0; i < EVENT_HANDLERS_TABLE_SIZE; i++)
			if (_etable[i])
				delete _etable[i];

		for (i = 0; i < ACTION_HANDLERS_TABLE_SIZE; i++)
			if (_atable[i])
				delete _atable[i];
	}

	int32 Block::notifyPeersOf(DataPin* dp)
	{
		cmessage cm;

		dp->lockTable(Pin::PEERS_TABLE);
		memset(&cm, 0, sizeof(cmessage));

		Enumeration* peers = dp->getPeers();
		while (peers->hasMoreElements()) {
			DataPin* dpi = (DataPin*) peers->nextElement();

			Block* b = dpi->getBlock();
			ControlPin* cp = b->getControlPin();

			cm.code = EVENT_DATA_READY;
			cm.from = this;

			cp->tryPutMessage(&cm);
		}

		dp->unlockTable(Pin::PEERS_TABLE);

		// ok
		return SUCCESS;
	}

	int32 Block::sendMessageToPeersOf(DataPin* dp, uint32 code)
	{
		cmessage cm;

		dp->lockTable(Pin::PEERS_TABLE);
		memset(&cm, 0, sizeof(cmessage));

		Enumeration* peers = dp->getPeers();
		while (peers->hasMoreElements()) {
			DataPin* dpi = (DataPin*) peers->nextElement();

			Block* b = dpi->getBlock();
			ControlPin* cp = b->getControlPin();

			cm.code = code;
			cm.from = this;

			cp->tryPutMessage(&cm);
		}

		dp->unlockTable(Pin::PEERS_TABLE);

		// ok
		return SUCCESS;
	}

	int32 Block::attachEventHandler(int32 event_id, BlockHandler* handler)
	{
		MutexLocker ml(&_mutexTable);

		if (event_id < 0 || event_id >= EVENT_HANDLERS_TABLE_SIZE) {
			return FAILURE;
		}

		_etable[event_id] = handler;

		// ok
		return SUCCESS;
	}

	BlockHandler* Block::detachEventHandler(int32 event_id)
	{
		BlockHandler* retval = NULL;

		MutexLocker ml(&_mutexTable);

		if (event_id < 0 || event_id >= EVENT_HANDLERS_TABLE_SIZE) {
			return NULL;
		}

		retval = _etable[event_id];
		_etable[event_id] = NULL;

		// ok
		return retval;
	}

	int32 Block::attachActionHandler(int32 action_id, BlockHandler* handler)
	{
		MutexLocker ml(&_mutexTable);

		if (action_id < 0 || action_id >= ACTION_HANDLERS_TABLE_SIZE) {
			return FAILURE;
		}

		_atable[action_id] = handler;

		// ok
		return SUCCESS;
	}

	BlockHandler* Block::detachActionHandler(int32 action_id)
	{
		MutexLocker ml(&_mutexTable);
		BlockHandler* retval = NULL;

		if (action_id < 0 || action_id >= ACTION_HANDLERS_TABLE_SIZE) {
			return NULL;
		}

		retval = _atable[action_id];
		_atable[action_id] = NULL;

		// ok
		return retval;
	}

	void Block::lockTable(int32 what)
	{
		switch (what) {
		case INPUT_TABLE:
			_idp.lock(); break;
		case OUTPUT_TABLE:
			_odp.lock(); break;
		default:
			_idp.lock();
			_odp.lock();
		}
	}

	void Block::unlockTable(int32 what)
	{
		switch (what) {
		case INPUT_TABLE:
			_idp.unlock(); break;
		case OUTPUT_TABLE:
			_odp.unlock(); break;
		default:
			_idp.unlock();
			_odp.unlock();
		}
	}
}
