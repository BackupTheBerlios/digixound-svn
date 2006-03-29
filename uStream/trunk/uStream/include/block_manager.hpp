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

#ifndef BLOCKMANAGER_HPP
#define BLOCKMANAGER_HPP

#include "thread.hpp"
#include "shared_hash.hpp"
#include "sharedvars.hpp"
#include "shared_timer.hpp"
#include "configtable.hpp"
#include "loggable.hpp"
#include "interp.hpp"
#include "constants.hpp"
#include "source.hpp"
#include "sink.hpp"
#include "filter.hpp"

namespace uStreamLib {
	/**
	 * This is the global block manager. This class
	 * controls allocation and deallocation of blocks. It provides
	 * methods to communicate with blocks and to manage the whole
	 * uStream architecture.
	 * The BlockManager is also a thread. As a thread it performs
	 * the role of active controller in that it can receive or
	 * send control messages to all blocks.
	 * BlockManager inherits from ConfigTable, so that it can store
	 * configuration parameters as strings (see ConfigTable class
	 * for info).
	 */
	class US_EXPORT BlockManager : public Thread, public ConfigTable,
	public Loggable {
	public:
		/**
		 * Lock/Unlock constants.
		 * Constants to use for locking specific internal tables
		 * when using enumerations.
		 */
		enum { BLOCKS_TABLE = 1, SOURCES_TABLE = 2, SINKS_TABLE = 3,
		FILTERS_TABLE = 4, ALL_TABLES = 5 };

		/**
		 * Messages accepted by BlockManager.
		 */
		enum { BM_MESSAGE_SHUTDOWN = 0x00fffffe,
		BM_MESSAGE_BLOCKQUIT = 0x00ffffff };

		/**
		 * Enum that defines version information.
		 * To be used with getVersion() methods.
		 */
		enum { VERSION_MAJOR = 1, VERSION_MINOR = 2, VERSION_MICRO = 3,
		VERSION_PATCH = 4 };

		/**
		 * Constructor.
		 */
		BlockManager(void);

		/**
		 * Destructor.
		 */
		virtual ~BlockManager(void);

		/**
		 * Build the BlockManager.
		 * @param appname the application's name. 
		 * @param log the logger's object.
		 */
		int32 init(char* appname, Logger* log);

		/**
		 * Lock specific internal table.
		 * Use this method when you get enumeration of blocks, sources,
		 * sinks and filters.
		 * @param table_id one of BLOCKS_TABLE, SOURCES_TABLE, SINKS_TABLE
		 * and FILTERS_TABLE.
		 */
		void lockTable(int32 table_id);

		/**
		 * Unlock specific internal table.
		 * Use this method when you get enumeration of blocks, sources,
		 * and sinks. This method unlocks the table.
		 * @param table_id one of BLOCKS_TABLE, SOURCES_TABLE, SINKS_TABLE
		 * and FILTERS_TABLE.
		 */
		void unlockTable(int32 table_id);

		/*********************** BLOCK MANAGEMENT ***********************/

		/**
		 * Add a Source into the Block Manager.
		 * @param b the block to register.
		 * @return SUCCESS or FAILURE if the block cannot be added.
		 */
		int32 addSource(Source* b);

		/**
		 * Remove a Source from the Block Manager.
		 * @param name name of the block to remove.
		 * @return SUCCESS or FAILURE if the block cannot be removed.
		 */
		int32 delSource(char* name, bool del = true);

		/**
		 * Get a Source.
		 * @param name source name.
		 * @return the requested source or NULL if it does not exist.
		 */
		Source* getSource(char* name)
		{
			return (Source *) _stb.get(name);
		}

		/**
		 * Get source count.
		 * @return count of sources.
		 */
		uint32 getSourceCount(void)
		{
			return _stb.getCount();
		}

		/**
		 * Add a Sink into the Block Manager.
		 * @param b the block to register.
		 * @return SUCCESS or FAILURE if the block cannot be added.
		 */
		int32 addSink(Sink* b);

		/**
		 * Remove a Sink from the Block Manager.
		 * @param name name of the block to remove.
		 * @return SUCCESS or FAILURE if the block cannot be removed.
		 */
		int32 delSink(char* name, bool del = true);

		/**
		 * Get a Sink.
		 * @param name sink name.
		 * @return the requested sink or NULL if it does not exist.
		 */
		Sink* getSink(char* name)
		{
			return (Sink *) _ktb.get(name);
		}

		/**
		 * Get sink count.
		 * @return count of sinks.
		 */
		uint32 getSinkCount(void)
		{
			return _ktb.getCount();
		}

		/**
		 * Add a Filter into the Block Manager.
		 * @param b the block to register.
		 * @return SUCCESS or FAILURE if the block cannot be added.
		 */
		int32 addFilter(Filter* b);

		/**
		 * Remove a Filter from the Block Manager.
		 * @param name name of the block to remove.
		 * @return SUCCESS or FAILURE if the block cannot be removed.
		 */
		int32 delFilter(char* name, bool del = true);

		/**
		 * Get a Filter.
		 * @param name filter name.
		 * @return the requested filter or NULL if it does not exist.
		 */
		Filter* getFilter(char* name)
		{
			return (Filter *) _ftb.get(name);
		}

		/**
		 * Get filter count.
		 * @return count of filters.
		 */
		uint32 getFilterCount(void)
		{
			return _ftb.getCount();
		}

		/**
		 * Get a Block. Sources, Sinks and Filters are Blocks of course.
		 * When adding one of these objects to the BlockManager, they
		 * are added to three tables plus one: a table for sources, one
		 * for sinks, one for filters and one for all of them.
		 * @param name name of the block to get.
		 * @return the block or NULL if the block does not exist.
		 */
		Block* getBlock(char* name)
		{
			return (Block *) _blocks.get(name);
		}

		/**
		 * Get all sources. Please, use lock() and unlock() methods
		 * when working on this enumeration.
		 * @return enumeration of blocks that act as sources.
		 */
		Enumeration* getSources(void)
		{
			return _stb.values();
		}

		/**
		 * Get all sinks. Please, use lock() and unlock() methods
		 * when working on this enumeration.
		 * @return enumeration of blocks that act as sinks.
		 */
		Enumeration* getSinks(void)
		{
			return _ktb.values();
		}

		/**
		 * Get all filters. Please, use lock() and unlock() methods
		 * when working on this enumeration.
		 * @return enumeration of blocks that act as filters.
		 */
		Enumeration* getFilters(void)
		{
			return _ftb.values();
		}

		/**
		 * Get all blocks dispite of being source, sink or filter.
		 * Please, use lock() and unlock() methods when working on
		 * this enumeration.
		 * @return enumeration of blocks.
		 */
		Enumeration* getBlocks(void)
		{
			return _blocks.values();
		}

		/***************** PIN CONNECTION METHODS *******************/

		/**
		 * Connect two pins.
		 * @param p1 first pin to connect.
		 * @param p2 second pin to connect.
		 * @return SUCCESS or FAILURE.
		 */
		int32 connectPins(DataPin* p1, DataPin* p2);

		/**
		 * Disconnect two pins.
		 * @param p1 first pin to disconnect.
		 * @param p2 second pin to disconnect.
		 * @return SUCCESS or FAILURE.
		 */
		int32 disconnectPins(DataPin* p1, DataPin* p2);

		/**
		 * Disconnect two pins by using the wire.
		 * @param wire the wire whose pins must be disconnected.
		 * @return SUCCESS or FAILURE.
		 */
		int32 disconnectPins(Wire* wire);

		/***************** BLOCK MESSAGE METHODS ********************/

		/**
		 * Send a control message to the selected block.
		 * This method blocks until message is sent. Don't use this method
		 * if you lock() the BLOCKS_TABLE.
		 * @param block_name name of the block to send message to.
		 * @param m message to send.
		 * @param SUCCESS or FAILURE if block does not exist.
		 */
		int32 sendMessage(char* block_name, cmessage* m);

		/**
		 * Send a control message to the selected block.
		 * This method tries to send the message. If the message
		 * cannot be sent, this method returns. Don't use this method
		 * if you lock() the BLOCKS_TABLE.
		 * @param block_name name of the block to send message to.
		 * @param m message to send.
		 * @param SUCCESS or FAILURE if message cannot be sent.
		 */
		int32 trySendMessage(char* block_name, cmessage* m);

		/**
		 * Send a control message to the selected block.
		 * This method sends a message specified as string. Don't use
		 * this method if you lock() the BLOCKS_TABLE.
		 * @param block_name name of the block to send message to.
		 * @param msg message string to send.
		 * @param wait true means wait for message to be put into the input queue of
		 * the receiving block.
		 * @return SUCCESS or FAILURE if message cannot be sent.
		 */
		int32 sendMessage(char* block_name, // block's name
	  			char* msg, // string message
				smessage* sm = NULL, // status message to enable status listeners
				bool wait = true     // flag: true means wait until the message is put
				);

		/**
		 * Send a control message to the selected block.
		 * This method sends a message specified as string. Don't use
		 * this method if you lock() the BLOCKS_TABLE.
		 * @param block block to send message to.
		 * @param msg message string to send.
		 * @return SUCCESS or FAILURE if message cannot be sent.
		 */
		int32 sendMessage(Block* block, char* msg, smessage* sm = NULL,
			bool wait = true)
		{
			return sendMessage(block->getName(), msg, sm, wait);
		}

		/**
		 * Get Block Manager control pin. It is not a Block but
		 * has a control pin so that can communicate with Blocks.
		 * @return internal control pin.
		 */
		ControlPin* getControlPin(void)
		{
			return &_cp;
		}
		
		/**
		 * Method used by blocks to store error information.
		 * @param error_string the error string.
		 */
		void setBlockErrorString(char *error_string)
		{
			_block_es = error_string;
		}

		/**
		 * Method used to get blocks' error string.
		 * When a block does its init(), this method returns
		 * the error string.
		 * @return an error string.
		 */
		char *getBlockErrorString(void)
		{
			return _block_es.get();
		}

		/**
		 * Set global error string.
		 * @param err error string.
		 */
		void setErrorString(char* err)
		{
			_error_string = err;
		}

		/**
		 * Get global error string.
		 * This error string is different from plugin error string.
		 * @return an error string.
		 */
		char* getErrorString(void)
		{
			return _error_string.get();
		}

		/**
		 * Reset global configuration parameters.
		 * This method set to default values all BlockManager
		 * properties.
		 */
		void resetProperties(void);

		/**
		 * Method to get version information about uStream.
		 * @param what specify major, minor, micro or patch.
		 * @return version number.
		 */
		static uint32 getVersion(int32 what);

		/**
		 * Method to get version string.
		 * @return the version string.
		 */
		static char* getVersion(void);

		/**
		 * Method to get global unique identifier counter.
		 * @return counter value.
		 */
		uint32 getGlobalCounter(void)
		{
			++_counter; return _counter.get();
		}

		/**
		 * Ask a time description for current time.
		 * @param td a pointer to a TimeDesc structure.
		 */
		void getClockTime(TimeDesc* td)
		{
			_clock.stop(td);
		}

		/**
		 * Block Manager entry point. This method performs all
		 * the actions needed for controlling the blocks.
		 */
		void run(void);
	private:
		/*
		 * Other constructors protected.
		 */
		BlockManager(BlockManager&)
		{
		}

		/*
		 * Size constants.
		 */
		enum { _VERSION_STRING_SZ = 256, };

		/* table of all blocks (pointers) */
		SharedHash _blocks;

		/* table of blocks that act as sources (pointers) */
		SharedHash _stb;

		/* table of blocks that act as sinks (pointers) */
		SharedHash _ktb;

		/* table of blocks that act as filters (pointers) */
		SharedHash _ftb;

		/* Timer used as synchornization clock */
		SharedTimer _clock;

		/* Block Manager's Control Pin */
		ControlPin _cp;
		
		/* Block's error string */
		SharedString _block_es;

		/* Global error string */
		SharedString _error_string;

		/* Termination semaphore */
		Semaphore _termSem;

		/* Version string */
		static char _version_string[_VERSION_STRING_SZ];

		/* Global unique identifier generator */
		SharedUint _counter;

		/* method to build property descriptors */
		void _buildPropertyDescriptions(void);
	};
}

#endif
