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

#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "thread.hpp"
#include "shared_hash.hpp"
#include "sharedvars.hpp"
#include "configtable.hpp"
#include "interp.hpp"
#include "block_handler.hpp"
#include "default_status_listener.hpp"

namespace uStreamLib {
	/* declares block manager */
	class BlockManager;

	/**
	 * This class is the basic block of a graph. Each block is specialized
	 * in performing a task such as reading from an audio or video device,
	 * writing to a video surface or audio output, injecting data into the
	 * network. A Block can act as a source, sink or filter.
	 * Each block runs into a separate thread and has a control pin
	 * on which it waits for control commands.
	 * Each block inherits from ConfigTable so that it can store
	 * configuration parameters as strings (see ConfigTable for info).
	 */
	class US_EXPORT Block : public Thread, public ConfigTable {
	public:
		/**
		 * Block types.
		 * This enum contains constants that define block types.
		 */
		enum BlockType { /** the block produces data (source) */
		TYPE_SOURCE = 1, /** the block filters data (filter) */
		TYPE_FILTER = 2, /** the block consumes data (sink) */
		TYPE_SINK = 3 };

		/**
		 * Action handlers identifiers. Each code is an index in the
		 * action handlers table, provided by a block.
		 */
		enum ActionID { /** invoked when source needs to produce data */
		ACTION_DATA_PRODUCE = 0,
		/** invoked when sink needs to consume data */
		ACTION_DATA_CONSUME = 1,
		/** invoked when filter needs to filter data */
		ACTION_DATA_FILTER = 2, /** last used action id plus 1 */
		ACTION_LAST_USED_ID = 3, /** maximum id for these indexes */
		ACTION_MAX_ID = 10 };

		/**
		 * Event handlers identifiers.
		 * Each code is an index in the event handler table,
		 * provided by a block. Each event has a meaning if you give
		 * it a meaning.
		 */
		enum EventID { /** make the block do a reset */
		EVENT_RESET = 0, /** start the block */
		EVENT_START = 1, /** stop the block */
		EVENT_STOP = 2, /** make the block do a pause */
		EVENT_PAUSE = 3, /** make the block bufferize the data */
		EVENT_BUFFERIZE = 4, /** make the block do a seek */
		EVENT_SEEK = 5, /** get total amount of data */
		EVENT_TELL = 6, /** make the block skip data */
		EVENT_SKIP = 7, /** quit this block */
		EVENT_QUIT = 8, /** data ready on input pins */
		EVENT_DATA_READY = 9, /** timeout event */
		EVENT_TIMEOUT = 10, /** terminate the block (stronger than quit) */
		EVENT_TERMINATE = 11, /** execute specific command */
		EVENT_COMMAND = 12, /** last used event id plus 1 */
		EVENT_LAST_USED_ID = 13, /** maximum id for these indexes */
		EVENT_MAX_ID = 20 };

		/**
		 * Status identifiers.
		 * Each id is returned by getStatus() method. Use
		 * getStatusString() to get a string rapresentation of these
		 * numbers.
		 */
		enum StatusID { /** the block is ready */
		STATUS_READY = 0, /** the block is started */
		STATUS_STARTED = 1, /** the block is stopped */
		STATUS_STOPPED = 2, /** the block is paused */
		STATUS_PAUSED = 3, /** the block is performing a seek */
		STATUS_SEEKING = 4, /** the block is performing a skip */
		STATUS_SKIPPING = 5, /** the block is bufferizing data */
		STATUS_BUFFERIZING = 6, /** the block is performing a reset */
		STATUS_RESETTING = 7, /** the block is quitting */
		STATUS_QUITTING = 8, /** the block is terminating */
		STATUS_TERMINATING = 9, /** the block is performing a tell */
		STATUS_TELLING = 10, /** the block is in an error condition */
		STATUS_ERROR = 11 };

		/**
		 * Number of event handler slots.
		 */
		enum HandlerTableSize { /** action handlers table size */
		ACTION_HANDLERS_TABLE_SIZE = ACTION_MAX_ID,
		/** event handlers table size */
		EVENT_HANDLERS_TABLE_SIZE = EVENT_MAX_ID };

		/**
		 * Return values for methods which execute an handler.
		 */
		enum ExecHandlerReturnValue { HANDLER_UNDEFINED = -1 };

		/**
		 * Lock/Unlock constants. Constants to use in lock/unlock.
		 */
		enum LockTableID { /** Lock input table */
		INPUT_TABLE = 1, /** Lock output table */
		OUTPUT_TABLE = 2, /** Lock all tables */
		ALL_TABLES = 3 };

		/**
		 * Constructor.
		 */
		Block(void);

		/**
		 * Destructor.
		 */
		virtual ~Block(void);

		/**
		 * Deallocate memory used by pins. Call this to delete all pins.
		 */
		void destroyPins(void);

		/**
		 * Deallocate memory for all action and event callbacks.
		 */
		void destroyHandlers(void);

		/**
		 * Get the Control Pin.
		 * @return a pointer to the internal control pin.
		 */
		ControlPin* getControlPin(void)
		{
			return &_cp;
		}

		/**
		 * Get Block type.
		 * @return block type, ie one of Block::TYPE_SOURCE,
		 * Block::TYPE_SINK or Block::TYPE_FILTER or FAILURE
		 * if the block type is not valid.
		 */
		virtual int32 getType(void)
		{
			return FAILURE;
		}

		/**
		 * Use this method to create a data pin. The returned
		 * datapin is already added to the right pins table (i.e.
		 * pins with output direction to output pins table and those
		 * with input direction to input pins table).
		 * @param name pin's name (used as identifier)
		 * @param dir pin's direction, one of DIR_INPUT or DIR_OUTPUT
		 * @param bufsz preferred buffer size.
		 * @param bufco preferred buffers count.
		 * @param queuesz preferred queue size.
		 * @return a pointer to the created pin.
		 */
		DataPin* createDataPin(char* name, uint8 dir, uint32 bufsz,
			uint32 bufco, int32 queuesz);

		/**
		 * Remove an input pin.
		 * @param pin_name pin name to remove.
		 * @return SUCCESS or FAILURE if the pin cannot be removed.
		 */
		int32 delInputPin(char* pin_name)
		{
			return _idp.del(pin_name);
		}

		/**
		 * Get all the input pins. When using the returned enumeration,
		 * you must lock the pins table.
		 * @return an enumeration of all input pins.
		 */
		Enumeration* getInputPins(void)
		{
			return _idp.values();
		}

		/**
		 * Get the specified input pin.
		 * @param name pin's name.
		 * @return a pointer to the requested pin or NULL.
		 */
		DataPin* getInputPin(char* name)
		{
			return (DataPin *) _idp.get(name);
		}

		/**
		 * Check if pin belongs to this block.
		 * @param p the pin we want to check.
		 * @return true or false.
		 */
		bool belongs(Pin* p);

		/**
		 * Remove an output pin.
		 * @param pin_name pin name to remove.
		 * @return SUCCESS or FAILURE if the pin cannot be removed.
		 */
		int32 delOutputPin(char* pin_name)
		{
			return _odp.del(pin_name);
		}

		/**
		 * Get all the output pins.
		 * @return an enumeration of all output pins.
		 */
		Enumeration* getOutputPins(void)
		{
			return _odp.values();
		}

		/**
		 * Get the specified output pin.
		 * @param name pin's name.
		 * @return a pointer to the requested pin or NULL.
		 */
		DataPin* getOutputPin(char* name)
		{
			return (DataPin *) _odp.get(name);
		}

		/**
		 * This method notifies of data arrival every peer connected
		 * to the specified data pin. Use this method to awake blocks
		 * so that they can process arriving data.
		 * @param dp data pin whose peers need to be awoken.
		 * @return SUCCESS or FAILURE.
		 */
		int32 notifyPeersOf(DataPin* dp);

		/**
		 * This method make a block able to send specific control
		 * messages to all connected peers for a specific data pin.
		 * For example, a block can stop, reset or control in some
		 * way all connected blocks in a chain.
		 * @param dp the data pin whose blocks must receive the
		 * message
		 * @param code the message to send (EVENT_* messages).
		 * @return SUCCESS or FAILURE if message cannot be sent.
		 */
		int32 sendMessageToPeersOf(DataPin* dp, uint32 code);

		/**
		 * Attach event handler to a specific event.
		 * You can create an event handler object and then
		 * attach it to a specific event by invoking this method.
		 * An event handler is a piece of code that will be run when
		 * an event occurs. Events are STOP, PAUSE, RESET and so
		 * on as stated by a specific enum defined in this block.
		 * @param event_id the event id to attach the handler to.
		 * @param handler the event handler to attach.
		 * @return SUCCESS or FAILURE if handler cannot be attached.
		 */
		int32 attachEventHandler(int32 event_id, BlockHandler* handler);

		/**
		 * Detach event handler.
		 * You can detach an event handler specifying the event id.
		 * @param event_id event identifier.
		 * @return the detached handler or NULL if it cannot be detached.
		 */
		BlockHandler* detachEventHandler(int32 event_id);

		/**
		 * Attach action handler to a specific event.
		 * You can create an action handler object and then
		 * attach it to a specific action by invoking this method.
		 * An action handler is a piece of code that will be run
		 * when a block needs to perform an action such as produce
		 * data (for a source), consume data (for a sink) or filter
		 * data (for a filter, of course).
		 * @param action_id the action id to attache the handler to.
		 * @param handler the action handler to attach.
		 * @return SUCCESS or FAILURE if handler cannot be attached.
		 */
		int32 attachActionHandler(int32 action_id, BlockHandler* handler);

		/**
		 * Detach action handler.
		 * You can detach an action handler specifying the action id.
		 * @param action_id event identifier.
		 * @return the detached handler or NULL if it cannot be detached.
		 */
		BlockHandler* detachActionHandler(int32 action_id);

		/**
		 * Execute the specified event handler.
		 * @param event_id the event handler id.
		 * @return the handler's return value.
		 */
		int32 executeEventHandler(int32 event_id);

		/**
		 * Execute the specified action handler.
		 * @param action_id the action handler id.
		 * @return the handler's return value.
		 */
		int32 executeActionHandler(int32 action_id);

		/**
		 * Get status.
		 * @return this block's status.
		 */
		int32 getStatus(void)
		{
			return _status.get();
		}

		/**
		 * Set status.
		 * @param status the status id.
		 */
		void setStatus(int32 status)
		{
			_status = status;
		}

		/**
		 * Utility method to get a Block::TYPE_* as a string.
		 * @param type_id one of TYPE_SOURCE, TYPE_FILTER, TYPE_SINK,...
		 * @return the corresponding string.
		 */
		static char* getTypeString(int32 type_id);

		/**
		 * Utility method to get a string rapresentation of block
		 * status.
		 */
		static char* getStatusString(int32 status);

		/**
		 * Utility method to get command strings.
		 * The returned array contains strings each of which
		 * can be used as command to send to a block.
		 * @param cmd_count pointer to an integer in which this method
		 * will store the size of the returned array.
		 * @return a pointer to an array of strings.
		 */
		static char** getCommandStrings(uint32* cmd_count);

		/**
		 * Get the Block Manager.
		 * @return the block manager.
		 */
		BlockManager* getBlockManager(void)
		{
			return _bm;
		}

		/**
		 * Method to set seek info structure.
		 * @param si pointer to the seek info structure.
		 */
		void setSeekInfo(seekinfo* si)
		{
			memcpy(&_seekinfo, si, sizeof(_seekinfo));
		}

		/**
		 * Method to get seek info structure as set by
		 * EVENT_SEEK and EVENT_SKIP events.
		 * @return a pointer to the seek info structure for
		 * this block.
		 */
		seekinfo* getSeekInfo(void)
		{
			return &_seekinfo;
		}

		/**
		 * Get general descriptive info about this block.
		 * @return a string.
		 */
		char* getInfo(void)
		{
			return _info.get();
		}

		/**
		 * Set general descriptive info about this block.
		 * @param info a string.
		 */
		void setInfo(char* info)
		{
			_info = info;
		}

		/**
		 * Get error string in case of error coditions.
		 * @return a pointer to the error string buffer.
		 */
		char* getErrorString(void)
		{
			return _errorstring.get();
		}

		/**
		 * Set error string. To be used by plugins programmers.
		 * Not useful for user interface programmers.
		 * @param error the error string to store into this block's
		 * error string buffer.
		 */
		void setErrorString(char* error)
		{
			_errorstring = error;
		}

		/**
		 * This method locks internal pins tables. You need to call
		 * this method when you are using enumerations of input or output
		 * pins.
		 * @param what which table to lock (INPUT_TABLE, OUTPUT_TABLE, ALL_TABLES).
		 */
		void lockTable(int32 what);

		/**
		 * Method to unlock specific tables.
		 * @param what which table to lock (INPUT_TABLE, OUTPUT_TABLE, ALL_TABLES).
		 */
		void unlockTable(int32 what);

		/**
		 * Produce a descriptive string of this block.
		 * @return a descriptive string for this block.
		 */
		char* toString(void);

	protected:
		/*
		 * Other constructors protected.
		 */
		Block(Block&)
		{
		}

		/**
		 * Initialize a block. A block has a descriptive name
		 * and one ControlPin. A Block can have input or output DataPins
		 * based on the specified type. A source has only output DataPins;
		 * a sink has only input DataPins and a filter has both input and
		 * output DataPins.
		 * @param bm a pointer to the block manager.
		 * @param name a descriptive name of this block.
		 * @param bufsz preferred control pin's buffer size.
		 * @param bufcount preferred control pin's buffers count.
		 * @param queuesz control pin's internal priority queue size.
		 */
		int32 init(BlockManager* bm, // pointer to block manager
	  		char* name, // descriptive name
			uint32 bufsz = US_CP_BUFSZ, // control pin's preferred buffer size
			uint32 bufcount = US_CP_BUFCO, // control pin's preferred buffer count
			int32 queuesz = US_CP_QUEUESZ  // control pin's queue size
		);

	private:
		/* static storage for command strings */
		static char* _cmdstrings[EVENT_HANDLERS_TABLE_SIZE];

		/* block manager pointer */
		BlockManager* _bm;

		/* the control pin */
		ControlPin _cp;

		/* input data pins table */
		SharedHash _idp;

		/* output data pins table */
		SharedHash _odp;

		/* databuf for toString() processing */
		DataBuf _tostring;

		/* table of registered event handlers */
		BlockHandler* _etable[EVENT_HANDLERS_TABLE_SIZE];

		/* table of registered action handlers */
		BlockHandler* _atable[ACTION_HANDLERS_TABLE_SIZE];

		/* handler and action mutex */
		Mutex _mutexTable;

		/* seek info */
		seekinfo _seekinfo;

		/* status */
		SharedInt _status;

		/* general descriptive info (fixed size) */
		SharedString _info;

		/* error string (fixed size) */
		SharedString _errorstring;
	};
}

#endif
