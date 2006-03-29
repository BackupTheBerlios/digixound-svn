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

#include "block_manager.hpp"
#include "source.hpp"

namespace uStreamLib {
	Source::Source(void)
	{
		Thread::setClassID(UOSUTIL_RTTI_SOURCE);
	}

	Source::~Source(void)
	{
		/*
		 * Log that someone is deleting me.
		 */
		Logger* l = getBlockManager()->getLogger();
		l->log(Logger::LEVEL_CRIT,
			"%s: waiting for QUIT SEMAPHORE to be unlocked...", getName());

		/*
		 * Wait on the semaphore which signals that
		 * the block can quit.
		 */
		_quitSem.wait();

		/*
		 * Log that delete is ok.
		 */
		l->log(Logger::LEVEL_CRIT, "%s: bye bye", getName());
	}

	int32 Source::init(BlockManager* bm, char* name, uint32 bufsz,
		uint32 bufcount, int32 queuesz)
	{
		int32 ret = 0;

		// initialize parent
		ret = Block::init(bm, name, bufsz, bufcount, queuesz);
		if (ret == FAILURE)
			return FAILURE;

		// initialize members
		_started = false;
		_quit = false;

		/*
		 * Create a semaphore which starts with the 0 value.
		 * The destructor for this block will block on this
		 * semaphore. Only the onPostSelect() method will post
		 * the semaphore when the _quit flag is true.
		 */
		ret = _quitSem.init(0);
		if (ret == FAILURE)
			return FAILURE;

		// ok
		return SUCCESS;
	}

	void Source::run(void)
	{
		int32 ret = 0, is_msg = 0, handler_ret = 0;
		cmessage cm;

		Logger* sl = getBlockManager()->getLogger();

		while (!_quit) {
			if (_started) {
				// get a message from control pin w/o blocking
				ret = getControlPin()->tryRecvMessage(&cm);
				if (ret == SUCCESS)
					is_msg = 1;
			} else {
				// get a message from control pin blocking
				ret = getControlPin()->recvMessage(&cm);
				if (ret == SUCCESS)
					is_msg = 1;
			}

			if (is_msg) {
				// detect terminate event
				if (cm.code == Block::EVENT_TERMINATE) {
					_started = false; _quit = true;
					sl->log(Logger::LEVEL_NOTICE,
							"%s: terminate request received", getName());

					// set status
					setStatus(STATUS_TERMINATING);

					// jump to while()
					//continue;  TODO: Is this needed?
				} else {
					// check message codes and set status
					switch (cm.code) {
					case EVENT_PAUSE:
						setStatus(STATUS_PAUSED);
						_started = false;
						sl->log(Logger::LEVEL_NOTICE,
								"%s: pause request received", getName());
						break;
					case EVENT_RESET:
						setStatus(STATUS_RESETTING);
						_started = false;
						sl->log(Logger::LEVEL_NOTICE,
								"%s: reset request received", getName());
						break;
					case EVENT_START:
						setStatus(STATUS_STARTED);
						_started = true;
						sl->log(Logger::LEVEL_NOTICE,
								"%s: start request received", getName());
						break;
					case EVENT_STOP:
						setStatus(STATUS_STOPPED);
						_started = false;
						sl->log(Logger::LEVEL_NOTICE,
								"%s: stop request received", getName());
						break;
					case EVENT_BUFFERIZE:
						setStatus(STATUS_BUFFERIZING);
						sl->log(Logger::LEVEL_NOTICE,
								"%s: bufferize request received (strange)",
								getName());
						break;
					case EVENT_SEEK:
						setStatus(STATUS_SEEKING);
						setSeekInfo(&cm.seek);
						sl->log(Logger::LEVEL_NOTICE,
								"%s: seek request received", getName());
						break;
					case EVENT_TELL:
						setStatus(STATUS_TELLING);
						sl->log(Logger::LEVEL_NOTICE,
								"%s: tell request received", getName());
						break;
					case EVENT_SKIP:
						setStatus(STATUS_SKIPPING);
						setSeekInfo(&cm.seek);
						sl->log(Logger::LEVEL_NOTICE,
								"%s: skip request received (strange)",
								getName());
						break;
					case EVENT_QUIT:
						setStatus(STATUS_QUITTING);
						_started = false; _quit = true;
						sl->log(Logger::LEVEL_NOTICE,
								"%s: quit request received", getName());
						break;
					case EVENT_TIMEOUT:
						sl->log(Logger::LEVEL_NOTICE,
								"%s: timeout request received", getName());
						break;
					case EVENT_DATA_READY:
						setStatus(STATUS_READY);
						_started = false;
						sl->log(Logger::LEVEL_NOTICE,
								"%s: mmh, data_ready request received",
								getName());
						break;
					case EVENT_COMMAND:
						sl->log(Logger::LEVEL_NOTICE,
								"%s: command request received", getName());
						break;
					default:
						if (cm.code >= EVENT_MAX_ID)
							sl->log(Logger::LEVEL_CRIT,
									"%s: invalid message code (%d)",
									getName(), cm.code);
						else
							sl->log(Logger::LEVEL_CRIT,
									"%s: user message %d received", getName(),
									cm.code);
					}

					// execute event handler for this message code
					handler_ret = executeEventHandler(cm.code);
					if (handler_ret == Block::HANDLER_UNDEFINED) {
						sl->log(Logger::LEVEL_NOTICE,
								"%s: undefined handler %d", getName(), cm.code);
					} else if (handler_ret == BlockHandler::HFAILURE) {
						_started = false; setStatus(STATUS_READY);
						sl->log(Logger::LEVEL_ERROR, "%s: handler %d failed",
								getName(), cm.code);
					}

					/*
					 * This block checks for StatusListeners availability. If
					 * any status listener has been registered, this block sends
					 * status messages to it.
					 */
					if (cm.status_listener) {
						// DEBUG
						UOSUTIL_DERR((stderr, "Using status listener\n"));

						smessage sm;
						memset(&sm, 0, sizeof(smessage));

						sm.code = cm.code;
						sm.serial = cm.serial;
						sm.status = getStatus();
						sm.eh_return = handler_ret;
						snprintf(sm.error, SM_MESSAGE_SZ, "%s",
							getErrorString());

						ret = cm.status_listener->notifyStatusMessage(&sm);
						if (ret == FAILURE) {
							sl->log(Logger::LEVEL_WARN,
									"%s: cannot notify status to registered listener",
									getName());
						}

						// DEBUG
						UOSUTIL_DERR((stderr, "Status listener done\n"));
					}

					// set status to ready if compatible
					if (cm.code != EVENT_START &&
						cm.code != EVENT_STOP &&
						cm.code != EVENT_PAUSE &&
						cm.code != EVENT_TELL &&
						cm.code != EVENT_COMMAND)
						setStatus(STATUS_READY);

					// wait for next message
					is_msg = 0;
				}
			}

			if (_started) {
				// do source main action: produce data
				handler_ret = executeActionHandler(ACTION_DATA_PRODUCE);
				if (handler_ret == Block::HANDLER_UNDEFINED) {
					_started = false; setStatus(STATUS_READY);
					sl->log(Logger::LEVEL_CRIT,
							"%s: undefined action handler", getName());
				} else if (handler_ret == BlockHandler::HSUCCESS) {
					// DO NOTHING HERE (useless log)
				} else if (handler_ret == BlockHandler::HFAILURE) {
					   	_started = false; setStatus(STATUS_READY);
					sl->log(Logger::LEVEL_ERROR,
					   		"%s: action handler returned FAILURE", getName());
				} else if (handler_ret == BlockHandler::HCRITICAL) {
					   	_started = false; setStatus(STATUS_READY);
					sl->log(Logger::LEVEL_EMERG,
					   		"%s: action handler returned CRITICAL FAILURE",
					   		getName());
				} else {
					sl->log(Logger::LEVEL_CRIT,
					   		"%s: action handler returned undefined value",
					   		getName());
				}
			}
		}

		// release the quit semaphore
		sl->log(Logger::LEVEL_CRIT,
				"%s: Releasing QUIT SEMAPHORE (so quitting)", getName());

		_quitSem.post();

		// log termination before block destruction
		sl->log(Logger::LEVEL_CRIT, "%s: Asking BlockManager for termination",
				getName());

		/*
		 * Notify the block manager that this block is quitting.
		 * Messages must be sent with normal priority since the
		 * block manager have to complete all pending activities.
		 */
		cmessage bmm;

		memset(&bmm, 0, sizeof(bmm));
		bmm.code = BlockManager::BM_MESSAGE_BLOCKQUIT;
		bmm.from = this;

		getControlPin()->sendMessage(&bmm);
	}
}
