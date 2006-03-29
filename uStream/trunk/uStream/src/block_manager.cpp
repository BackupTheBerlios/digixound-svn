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

#include "block_manager.hpp"

namespace uStreamLib {
	/*
	 * Parameter callbacks for Block Manager.
	 */

	class LoggerLevel : public ConfigCallBack {
	public:
		LoggerLevel(BlockManager* bm)
			: _bm(bm)
		{
			int32 ret = ConfigCallBack::init(USBM_LOGGER_LEVEL);
			if (ret == FAILURE) {
				fprintf(stderr, "Cannot initialize LoggerLevel callback.\n");
			}
		}

		virtual ~LoggerLevel(void)
		{
			// nothing to do
		}

		int32 perform(void*)
		{
			uint8 btLevel = *((uint8*) ival);

			_bm->getLogger()->setLevel(btLevel);
			_bm->getLogger()->log(Logger::LEVEL_EMERG,
								"Log level changed to %d", *ival);

			return SUCCESS;
		}
	private:
		/* the block manager */
		BlockManager* _bm;
	};

	/*
	* Block Manager implementation.
	*/

	char BlockManager::_version_string[BlockManager::_VERSION_STRING_SZ];

	BlockManager::BlockManager(void)
	{
		Thread::setClassID(UOSUTIL_RTTI_BLOCK_MANAGER);
	}

	BlockManager::~BlockManager(void)
	{
		cmessage cm;
		int32 ret = 0;

		// DEBUG
		UOSUTIL_DOUT(("~BlockManager(): entered\n"));
		UOSUTIL_DOUT(("%s: sending termination message to BM\n", getName()));

		// send termination message to this thread
		memset(&cm, 0, sizeof(cm));

		cm.code = BM_MESSAGE_SHUTDOWN;
		cm.from = NULL;

		ret = _cp.putMessage(&cm);
		if (ret == FAILURE) {
			log(Logger::LEVEL_ERROR,
				"Cannot send termination message to block manager");
		}

		// DEBUG
		UOSUTIL_DOUT(("%s: waiting for block manager termination...\n",
			getName()));

		// wait on termination semaphore
		_termSem.wait();

		// signal termination and delete logger
		log(Logger::LEVEL_EMERG, "uStream successfully shutdown");

		// DEBUG
		UOSUTIL_DOUT(("~BlockManager(): done\n"));
	}

	int32 BlockManager::init(char* appname, Logger* logger)
	{
		int32 ret = 0;

		// initialize parent thread
		ret = Thread::init(appname);
		if (ret == FAILURE)
			return FAILURE;

		// initialize config table
		ret = ConfigTable::init(US_CONFIGTABLE_HSIZE);
		if (ret == FAILURE)
			return FAILURE;

		// initialize error string
		ret = _error_string.init(US_BLOCK_ERRORSTRINGSZ);
		if (ret == FAILURE)
			return FAILURE;

		// initialize global counter
		ret = _counter.init(0);
		if (ret == FAILURE)
			return FAILURE;

		// detach thread
		detach();

		// initialize loggable
		setLogger(logger);

		// log that ustream is starting
		log(Logger::LEVEL_EMERG, "uStream Streaming Architecture 1 starting");

		// create table to contain all blocks
		ret = _blocks.init(US_BLOCKTABLE_HSIZE);
		if (ret == FAILURE)
			return FAILURE;

		// create table to contain only sources
		ret = _stb.init(US_BLOCKTABLE_HSIZE);
		if (ret == FAILURE)
			return FAILURE;

		// create table to contain only sinks
		ret = _ktb.init(US_BLOCKTABLE_HSIZE);
		if (ret == FAILURE)
			return FAILURE;

		// create timer (the clock)
		ret = _clock.init();
		if (ret == FAILURE)
			return FAILURE;

		_clock.start();

		// create dispatcher's control pin
		ret = _cp.init((Block *) NULL, US_CP_BUFSZ, US_CP_BUFCO, US_CP_QUEUESZ);
		if (ret == FAILURE)
			return FAILURE;

		// create termination semaphore
		ret = _termSem.init(0);
		if (ret == FAILURE)
			return FAILURE;

		// initialize global unique identifier generator (for block names)
		_counter = 0;

		// register and reset global configuration properties
		resetProperties();

		// create parameter callbacks (CREATE HERE)
		LoggerLevel* ll = new LoggerLevel(this);

		// register and attach parameter callbacks (REGISTER HERE)
		attachWrite(USBM_LOGGER_LEVEL, ll, NULL);

		/*
			 * create property extended descriptors
			 * -- must be called after resetProperties()
			 */
		_buildPropertyDescriptions();

		// signal initialization ok
		log(Logger::LEVEL_EMERG, "uStream successfully initialized");
		log(Logger::LEVEL_EMERG, "Application: %s", appname);
		log(Logger::LEVEL_EMERG, "uStream: %s", getVersion());

		// ok
		return SUCCESS;
	}

	int32 BlockManager::sendMessage(char* block_name, cmessage* m)
	{
		Block* b = getBlock(block_name);
		if (!b)
			return FAILURE;

		ControlPin* cp = b->getControlPin();
		return cp->putMessage(m);
	}

	int32 BlockManager::trySendMessage(char* block_name, cmessage* m)
	{
		Block* b = getBlock(block_name);
		if (!b)
			return FAILURE;

		ControlPin* cp = b->getControlPin();
		return cp->tryPutMessage(m);
	}

	int32 BlockManager::connectPins(DataPin* p1, DataPin* p2)
	{
		Wire* w = NULL;
		char tmp[4096];
		int32 ret = 0;

		// check if pins are null
		if (!p1) {
			log(Logger::LEVEL_EMERG, "%s: connect: pin1 is null", getName());
			return FAILURE;
		}

		if (!p2) {
			log(Logger::LEVEL_EMERG, "%s: connect: pin2 is null", getName());
			return FAILURE;
		}

		// create wire
		w = new Wire();
		if (!w)
			return FAILURE;

		ret = w->init(Wire::UNIDIRECTIONAL);
		if (ret == FAILURE) {
			// log errors
			log(Logger::LEVEL_ERROR, "%s: cannot connect pin %s to pin %s.",
				getName(), p1->getAbsoluteName(), p2->getAbsoluteName());

			// free resources
			delete w;

			// failure
			return FAILURE;
		}

		// connect pins using this wire
		ret = w->connect(p1->getBlock(), p1, p2->getBlock(), p2);
		if (ret == FAILURE) {
			snprintf(tmp, sizeof(tmp),
				"Error connecting pin %s to pin %s\n"
				"%s",
				p1->getAbsoluteName(), p2->getAbsoluteName(),
				w->getErrorString());
			_error_string = tmp;

			// log errors
			log(Logger::LEVEL_ERROR,
				"%s: cannot connect pin %s to pin %s: %s", getName(),
				p1->getAbsoluteName(), p2->getAbsoluteName(),
				w->getErrorString());

			// remove useless wire
			delete w;

			// failure
			return FAILURE;
		}

		// log notice
		log(Logger::LEVEL_NOTICE, "%s: connect(%s,%s) ok", getName(),
			p1->getAbsoluteName(), p2->getAbsoluteName());

		// ok
		return SUCCESS;
	}

	int32 BlockManager::disconnectPins(DataPin* p1, DataPin* p2)
	{
		Wire* w = NULL;

		// check if pins are null
		if (!p1) {
			log(Logger::LEVEL_EMERG, "%s: disconnect: pin1 is null", getName());
			return FAILURE;
		}

		if (!p2) {
			log(Logger::LEVEL_EMERG, "%s: disconnect: pin2 is null", getName());
			return FAILURE;
		}

		// get the wire that connects p1 to p2
		w = p1->getWire(p2);
		if (w) {
			// delete this wire
			delete w;

			// log notice
			log(Logger::LEVEL_WARN, "%s: disconnect(%s,%s) ok", getName(),
				p1->getAbsoluteName(), p2->getAbsoluteName());

			return SUCCESS;
		}

		// log notice
		log(Logger::LEVEL_WARN,
			"%s: strange, no wire between pin %s and pin %s", getName(),
			p1->getAbsoluteName(), p2->getAbsoluteName());

		// failure (no wire)
		return FAILURE;
	}

	int32 BlockManager::disconnectPins(Wire* w)
	{
		if (w) {
			// log notice
			log(Logger::LEVEL_WARN, "%s: disconnect(%s,%s) ok", getName(),
				w->getPin(1)->getAbsoluteName(),
				w->getPin(2)->getAbsoluteName());

			// delete this wire
			delete w;

			// ok
			return SUCCESS;
		}

		// log notice
		log(Logger::LEVEL_WARN, "%s: strange, null wire received", getName());

		// failure (no wire)
		return FAILURE;
	}

	void BlockManager::lockTable(int32 table_id)
	{
		switch (table_id) {
		case BLOCKS_TABLE:
			_blocks.lock(); break;
		case SOURCES_TABLE:
			_stb.lock();	break;
		case SINKS_TABLE:
			_ktb.lock();	break;
		case FILTERS_TABLE:
			_ftb.lock();	break;
		case ALL_TABLES:
			_blocks.lock();
			_stb.lock();
			_ktb.lock();
			_ftb.lock();
			break;
		}
	}

	void BlockManager::unlockTable(int32 table_id)
	{
		switch (table_id) {
		case BLOCKS_TABLE:
			_blocks.unlock(); break;
		case SOURCES_TABLE:
			_stb.unlock();    break;
		case SINKS_TABLE:
			_ktb.unlock();    break;
		case FILTERS_TABLE:
			_ftb.unlock();    break;
		case ALL_TABLES:
			_blocks.unlock();
			_stb.unlock();
			_ktb.unlock();
			_ftb.unlock();
			break;
		}
	}

	void BlockManager::resetProperties(void)
	{
		// setup basic properties
		setInt(USBM_ACTIONSCHEDULER_TIMEOUT, US_DEFAULT_BM_ASTIMEOUT);
		setInt(USBM_LOGGER_LEVEL, US_DEFAULT_BM_LOGLEVEL);
	}

	int32 BlockManager::addSource(Source* b)
	{
		int32 ret;
		char* bname = b->getName();

		ret = _blocks.pput(bname, (char *) b);
		if (ret != SUCCESS) {
			log(Logger::LEVEL_ERROR,
				"Error adding source \"%s\": already present (%d)", bname, ret);
			return FAILURE;
		}

		ret = _stb.pput(bname, (char *) b);
		if (ret != SUCCESS) {
			log(Logger::LEVEL_ERROR,
				"Error adding source: already present (%d)", ret);
			_blocks.del(bname); return FAILURE;
		}

		ControlPin* cp = b->getControlPin();
		Wire* w = new Wire();
		if (!w)
			return FAILURE;

		ret = w->init(Wire::BIDIRECTIONAL);
		if (ret == FAILURE) {
			delete w; return FAILURE;
		}

		ret = w->connect(b, cp, NULL, &_cp);
		if (ret == FAILURE) {
			log(Logger::LEVEL_ERROR, "Error adding source: %s",
				w->getErrorString());

			_blocks.del(bname);
			_stb.del(bname);

			return FAILURE;
		}

		log(Logger::LEVEL_WARN, "Source \"%s\" added (info=\"%s\")", bname,
			b->getInfo());

		return SUCCESS;
	}

	int32 BlockManager::delSource(char* name, bool del)
	{
		int32 ret;
		char* bname = NULL;

		Block* b = (Block*) _blocks.get(name);
		if (!b)
			return FAILURE;

		bname = b->getName();

		ret = _blocks.del(bname);
		if (ret == FAILURE)
			return FAILURE;

		ret = _stb.del(bname);
		if (ret == FAILURE)
			return FAILURE;

		log(Logger::LEVEL_WARN, "Source \"%s\" deleted", bname);

		if (del)
			delete b;
		return SUCCESS;
	}

	int32 BlockManager::addSink(Sink* b)
	{
		int32 ret;
		char* bname = b->getName();

		ret = _blocks.pput(bname, (char *) b);
		if (ret != SUCCESS) {
			log(Logger::LEVEL_ERROR,
				"Error adding sink \"%s\": already present (%d)", bname, ret);
			return FAILURE;
		}

		ret = _ktb.pput(bname, (char *) b);
		if (ret != SUCCESS) {
			log(Logger::LEVEL_ERROR,
				"Error adding sink: already present (%d)", ret);
			_blocks.del(bname); return FAILURE;
		}

		ControlPin* cp = b->getControlPin();
		Wire* w = new Wire();
		if (!w)
			return FAILURE;

		ret = w->init(Wire::BIDIRECTIONAL);
		if (ret == FAILURE) {
			delete w; return FAILURE;
		}

		ret = w->connect(b, cp, NULL, &_cp);
		if (ret == FAILURE) {
			log(Logger::LEVEL_ERROR, "Error adding sink: %s",
				w->getErrorString());
			_blocks.del(bname); _ktb.del(bname); return FAILURE;
		}

		log(Logger::LEVEL_WARN, "Sink \"%s\" added (info=\"%s\")", bname,
			b->getInfo());

		return SUCCESS;
	}

	int32 BlockManager::delSink(char* name, bool del)
	{
		int32 ret;
		char* bname = NULL;

		Block* b = (Block*) _blocks.get(name);
		if (!b)
			return FAILURE;

		bname = b->getName();

		ret = _blocks.del(bname);
		if (ret == FAILURE)
			return FAILURE;

		ret = _ktb.del(bname);
		if (ret == FAILURE)
			return FAILURE;

		log(Logger::LEVEL_WARN, "Sink \"%s\" deleted", bname);

		if (del)
			delete b; return SUCCESS;
	}

	int32 BlockManager::addFilter(Filter* b)
	{
		int32 ret;
		char* bname = b->getName();

		ret = _blocks.pput(bname, (char *) b);
		if (ret != SUCCESS) {
			log(Logger::LEVEL_ERROR,
				"Error adding filter \"%s\": already present (%d)", bname, ret);
			return FAILURE;
		}

		ret = _ftb.pput(bname, (char *) b);
		if (ret != SUCCESS) {
			log(Logger::LEVEL_ERROR,
				"Error adding filter: already present (%d)", ret);
			_blocks.del(bname); return FAILURE;
		}

		ControlPin* cp = b->getControlPin();
		Wire* w = new Wire();
		if (!w)
			return FAILURE;

		ret = w->init(Wire::BIDIRECTIONAL);
		if (ret == FAILURE) {
			delete w; return FAILURE;
		}

		ret = w->connect(b, cp, NULL, &_cp);
		if (ret == FAILURE) {
			log(Logger::LEVEL_ERROR, "Error adding filter: %s",
				w->getErrorString());

			_blocks.del(bname);
			_ftb.del(bname);

			return FAILURE;
		}

		log(Logger::LEVEL_WARN, "Filter \"%s\" added (info=\"%s\")", bname,
			b->getInfo());

		return SUCCESS;
	}

	int32 BlockManager::delFilter(char* name, bool del)
	{
		int32 ret;
		char* bname = NULL;

		Block* b = (Block*) _blocks.get(name);
		if (!b)
			return FAILURE;

		bname = b->getName();

		ret = _blocks.del(bname);
		if (ret == FAILURE)
			return FAILURE;

		ret = _ftb.del(bname);
		if (ret == FAILURE)
			return FAILURE;

		log(Logger::LEVEL_WARN, "Filter \"%s\" deleted", bname);

		if (del)
			delete b; return SUCCESS;
	}

	int32 BlockManager::sendMessage(char* block_name, char* msg, smessage* sm,
		bool wait)
	{
		cmessage cm;
		int32 ret = 0;

		uint32 cmd_count = 0;
		char** cmd = Block::getCommandStrings(&cmd_count);

		memset(&cm, 0, sizeof(cmessage));

		Block* b = getBlock(block_name);
		if (!b)
			return FAILURE;

		DefaultStatusListener sl(b);

		for (uint32 i = 0; i < cmd_count; i++) {
			if (!strcmp(cmd[i], msg)) {
				cm.code = i;
				cm.bid = 0;
				cm.from = NULL;
				cm.status_listener = sm ? &sl : NULL;

				log(Logger::LEVEL_WARN, "Sending message %s to %s (%s)",
					cmd[i], b->getName(), wait ? "wait" : "nowait");

				if (wait) {
					ret = b->getControlPin()->putMessage(&cm);
					if (ret == FAILURE) {
						log(Logger::LEVEL_ERROR, "PutMessage failed");
						return FAILURE;
					}
				} else {
					ret = b->getControlPin()->tryPutMessage(&cm);
					if (ret == FAILURE) {
						log(Logger::LEVEL_ERROR,
							"TryPutMessage failed (queue full)");
						return FAILURE;
					}
				}

				if (sm) {
					log(Logger::LEVEL_WARN, "Waiting for status message");
					ret = sl.getStatusMessage(sm, true);
					if (ret == FAILURE) {
						log(Logger::LEVEL_WARN, "GetStatusMessage failed");
						return FAILURE;
					}
				}

				break;
			}
		}

		return ret;
	}

	void BlockManager::run(void)
	{
		cmessage cm;
		int32 ret, stop = 0, msg_recvd = 0;
		int32 timeout = 0;
		int32 termination_request = 0, force_stop = 20;

		while (!stop) {
			// DEBUG
			// printf("%s: checking (stop=%d)...\n",getName(),stop);

			// get action scheduler timeout
			ret = getInt(USBM_ACTIONSCHEDULER_TIMEOUT, &timeout);
			if (ret == FAILURE)
				timeout = US_DEFAULT_BM_ASTIMEOUT;

			// wait for action scheduler timeout
			// printf("%s: sleeping (stop=%d)...\n",getName(),stop);
			Thread::sleep(timeout);
			// printf("%s: sleeped (stop=%d)...\n",getName(),stop);

			/*
						 * Check for a message coming from any thread.
						 * Notice that the invoked method is tryGetMessage()
						 * which does not check if the pin is connected. Any thread
						 * (block or block manager itself) can put message into this
						 * control pin.
						 */
			// printf("%s: checking queue (stop=%d)...\n",getName(),stop);
			ret = _cp.tryGetMessage(&cm);
			if (ret == SUCCESS)
				msg_recvd = 1;

			if (msg_recvd) {
				// DEBUG
				UOSUTIL_DOUT(("%s: message (stop=%d)...\n", getName(), stop));

				// execute message code
				switch (cm.code) {
				case BM_MESSAGE_SHUTDOWN:
					// log this message
					log(Logger::LEVEL_CRIT, "%s: MESSAGE(SHUTDOWN[%d],%s)",
						getName(), cm.code,
						cm.from ? cm.from->getName() : "unset");

					termination_request = 1;
					break;
				case BM_MESSAGE_BLOCKQUIT:
					// log this message
					log(Logger::LEVEL_CRIT, "%s: MESSAGE(BLOCKQUIT[%d],%s)",
						getName(), cm.code,
						cm.from ? cm.from->getName() : "unset");

					if (cm.from) {
						char* bname = cm.from->getName();
						switch (cm.from->getType()) {
						case Block::TYPE_SOURCE:
							delSource(bname, true); break;
						case Block::TYPE_SINK:
							delSink(bname, true); break;
						case Block::TYPE_FILTER:
							delFilter(bname, true); break;
						default:
							fprintf(stderr,
								"******** INVALID BLOCK TYPE ********");
							fprintf(stderr,
								"********        %d          ********",
								cm.from->getType());
						}
					}
					break;
				}

				// reset control message indicator
				msg_recvd = 0;
			}

			// printf("%s: checking termination request (stop=%d)...\n",getName(),stop);

			if (termination_request) {
				// check if there are active blocks
				if (!_blocks.getCount() || !force_stop) {
					stop = 1; log(Logger::LEVEL_NOTICE,
								"Block Manager quitting");
				} else {
					log(Logger::LEVEL_NOTICE, "Waiting for blocks termination");
					force_stop--;
				}
			}
		}

		// DEBUG
		UOSUTIL_DOUT(("%s: stopped\n", getName()));

		// unlock termination semaphore
		_termSem.post();
	}

	uint32 BlockManager::getVersion(int32 what)
	{
		switch (what) {
		case 1:
			return USTREAM_VERSION_MAJOR;
		case 2:
			return USTREAM_VERSION_MINOR;
		case 3:
			return USTREAM_VERSION_MICRO;
		case 4:
			return USTREAM_VERSION_PATCH;
		}

		return USTREAM_VERSION_MAJOR;
	}

	char* BlockManager::getVersion(void)
	{
		snprintf(_version_string, _VERSION_STRING_SZ, "%d.%d.%d-%02d",
			getVersion(VERSION_MAJOR), getVersion(VERSION_MINOR),
			getVersion(VERSION_MICRO), getVersion(VERSION_PATCH));

		return _version_string;
	}

	void BlockManager::_buildPropertyDescriptions(void)
	{
		Property* prop = NULL;

		prop = createPropertyDescription(USBM_LOGGER_LEVEL);
		if (prop) {
			prop->setAllowedMinInteger(Logger::LEVEL_DEBUG);
			prop->setAllowedMaxInteger(Logger::LEVEL_EMERG);
			prop->setDescription("Level of logging details (0 means debug)");
		}

		prop = createPropertyDescription(USBM_ACTIONSCHEDULER_TIMEOUT);
		if (prop) {
			prop->setAllowedMinInteger(10);
			prop->setAllowedMaxInteger(1000);
			prop->setDescription("Timeout for action scheduler to take an action");
		}
	}
}
