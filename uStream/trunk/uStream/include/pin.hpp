/*
  DANUBIO STREAMING ARCHITECTURE
  Copyright (C) 2000-2002 Michele Iacobellis (miacobellis@linuximpresa.it)
  Copyright (C) 2003-2004 Michele Iacobellis (m.iacobellis@nexotech.it)

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

#ifndef PIN_HPP
#define PIN_HPP

#include "shared_hash.hpp"
#include "configtable.hpp"
#include "types.hpp"
#include "message.hpp"
#include "wire.hpp"
#include "formats_video.hpp"
#include "formats_audio.hpp"
#include "formats_text.hpp"
#include "formats_byte.hpp"

namespace uStreamLib {
	/*
	 * Declare Block.
	 */
	class Block;

	/**
	 * This class is a Pin for connecting Blocks together.
	 * Two Pins must be connected using a Wire. A Pin must
	 * know the amount of data that wishes to manage. When
	 * you create a Pin, you must specify the preferred size
	 * and count of buffers this pin wants to use.
	 * Each Pin contains a buffer pool on which it can only do
	 * read operations (methods: use() and freeBuffer()). For
	 * write operations you need to get the connected Peer for
	 * this pin and then get its buffer pool (methods: getBuffer()).
	 */
	class US_EXPORT Pin : public Mutex, public ConfigTable {
	public:
		friend class Wire;
		friend class Block;

		/**
		 * Pin status values.
		 */
		enum { /** the pin is not connected */
		UNCONNECTED = 0, /** the pin is connected */
		CONNECTED = 1 };

		/**
		 * Pin direction.
		 */
		enum { /** the pin is for input */
		DIR_INPUT = 1, /** the pin is for output */
		DIR_OUTPUT = 2, /** the pin is for input/output */
		DIR_IO = 3 };

		/**
		 * Constants to use in lock/unlock.
		 */
		enum { /** lock/unlock the peers table */
		PEERS_TABLE = 1, /** lock/unlock the wires table */
		WIRES_TABLE = 2, /** lock/unlock all tables */
		ALL_TABLES = 3 };

		/**
		 * This union defines a subtype for this pin.
		 * If data type is byte stream, this union contains
		 * the *_UNDEF value for each subtype.
		 */
		typedef union {
			/**
			 * If data type for this pin is video,
			 * use this field for detecting video subtype.
			 */
			VideoFormat vf;

			/**
			 * If data type for this pin is audio,
			 * use this field for detecting audio subtype.
			 */
			AudioFormat af;

			/**
			 * If data type for this pin is text,
			 * use this field for detecting text subtype.
			 */
			TextFormat tf;

			/**
			 * If data type for this pin is bytes,
			 * use this field for detecting bytestream subtype.
			 */
			ByteStreamFormat bf;
		}
   SubType;

		/**
		 * Constructor.
		 */
		Pin(void);

		/**
		 * Destructor.
		 * Notice that this method deletes the connected
		 * Wire.
		 */
		virtual ~Pin(void);

		/**
		 * Get parent Block.
		 * @return pointer to the block.
		 */
		Block* getBlock(void)
		{
			return _block;
		}

		/**
		 * Get this pin's name.
		 * @return pin's name.
		 */
		char* getName(void)
		{
			return _name.toString();
		}

		/**
		 * Get absolute name of this pin. The absolute name
		 * is a string like "parent block name.pin name".
		 */
		char* getAbsoluteName(void)
		{
			return _absname.toString();
		}

		/**
		 * Get direction.
		 * @return pin direction.
		 */
		uint8 getDirection(void)
		{
			return _direction;
		}

		/**
		 * Get pin status.
		 * @return pin status.
		 */
		int32 getStatus()
		{
			return _status;
		}

		/**
		 * Set pin status.
		 * @param status one of CONNECTED, UNCONNECTED.
		 */
		void setStatus(int status)
		{
			_status = status;
		}

		/**
		 * Get preferred buffer size.
		 * @return preferred buffer size.
		 */
		uint32 getPreferredBufferSize(void)
		{
			return _pref_bufsz;
		}

		/**
		 * Get preferred buffers count.
		 * @return preferred buffers count.
		 */
		uint32 getPreferredBuffersCount(void)
		{
			return _pref_bufcount;
		}

		/**
		 * Get real buffer size.
		 * @return real buffer size.
		 */
		uint32 getRealBufferSize(void)
		{
			return _real_bufsz;
		}

		/**
		 * Get real buffers count.
		 * @return real buffers count.
		 */
		uint32 getRealBuffersCount(void)
		{
			return _real_bufcount;
		}

		/**
		 * Connect this pin to another pin. This method
		 * must be called when connect() is overridden.
		 * @param b the block whose pin we wish to connect to.
		 * @param peer the Pin to connect this pin to.
		 * @return an integer.
		 */
		virtual int32 connect(Block* b, Pin* peer);

		/**
		 * Disconnect this pin from another pin. This method
		 * must be called when disconnect() is overridden.
		 * @param b the block whose pin we wish to disconnect from.
		 * @param peer the Pin to disconnect this pin from.
		 * @return an integer.
		 */
		virtual int32 disconnect(Block* b, Pin* peer);

		/**
		 * Check if this pin is connected to the specified one.
		 * @param peer the pin to check for connection.
		 * @return true or false.
		 */
		virtual bool isConnectedTo(Pin* peer)
		{
			return (_peers.get(peer->getAbsoluteName()) != NULL);
		}

		/**
		 * Get count of connected peers.
		 * @return count.
		 */
		uint32 getPeersCount(void)
		{
			return _peers.getCount();
		}

		/**
		 * Get all peers.
		 * This method returns the Pins it is connected to.
		 * @return enumeration on peer pins.
		 */
		Enumeration* getPeers(void)
		{
			return _peers.values();
		}

		/**
		 * Get the Wire that connects this pin to the specified peer.
		 * @param the peer pin.
		 * @return NULL if unconnected.
		 */
		Wire* getWire(Pin* peer)
		{
			return (Wire *) _wires.get(peer->getAbsoluteName());
		}

		/**
		 * Get all the wires. Enumeration is empty if this pin is
		 * not connected.
		 * @return an enumeration of wires.
		 */
		Enumeration* getWires(void)
		{
			return _wires.values();
		}

		/**
		 * Get the specified input buffer.
		 * This method returns a databuf whose bid is specified as
		 * parameter. The returned databuf is ready for reading and
		 * has certainly been sent by the peer data pin.
		 * @param bid the buffer identifier of the requested buffer.
		 * @return the requested databuf or NULL if databuf cannot
		 * be returned.
		 */
		DataBuf* getInputBuffer(uint32 bid)
		{
			DataBuf* retval = NULL;
			if (_ibp)
				retval = _ibp->use(bid);

			return retval;
		}

		/**
		 * Free the specified input buffer.
		 * This method must be invoked when an input buffer as been
		 * used and the user wishes to make it available for further
		 * processing by peer's data pin.
		 * @param bid the buffer identifier of the buffer to free.
		 */
		void freeInputBuffer(uint32 bid)
		{
			if (_ibp)
				_ibp->freeBuffer(bid);
		}

		/**
		 * Get data type.
		 * @return the managed data type.
		 */
		DataType getDataType(void)
		{
			return _dt;
		}

		/**
		 * Set data type.
		 * @param dt data type.
		 */
		void setDataType(DataType dt)
		{
			_dt = dt;
		}

		/**
		 * Get subtype.
		 * @return a pointer to the subtype union.
		 */
		SubType* getSubType(void)
		{
			return &_subtype;
		}

		/**
		 * Method to lock specific tables.
		 * @param what use one of PEERS_TABLE and WIRES_TABLE.
		 */
		void lockTable(int32 what);

		/**
		 * Method to unlock specific tables.
		 * @param what use one of PEERS_TABLE and WIRES_TABLE.
		 */
		void unlockTable(int32 what);

		/**
		 * Method to get a string rapresentation of data type.
		 * @param dt data type.
		 * @param a string form of the specified data type.
		 */
		static char* getDataTypeString(DataType dt);
	protected:
		/*
		 * Other constructors protected.
		 */
		Pin(Pin&)
		{
		}

		/**
		 * Get next free buffer.
		 * This method blocks until a buffer is available.
		 * @return a pointer to next free buffer or NULL if an error
		 * occurred (pin disconnected at runtime).
		 */
		DataBuf* getFreeBuffer(void)
		{
			if (_ibp)
				return _ibp->use(_ibp->getBuffer()); return NULL;
		}

		/**
		 * Try to get next free buffer.
		 * This method will not block.
		 * @return a pointer to next free buffer or NULL if buffer
		 * is not available.
		 */
		DataBuf* tryGetFreeBuffer(void)
		{
			DataBuf* retval = NULL;

			uint32 bid = 0;
			int32 ret = 0;

			if (_ibp) {
				ret = _ibp->tryGetBuffer(&bid);
				if (ret == FAILURE)
					retval = NULL;
				else
					retval = _ibp->use(bid);
			} else
				retval = NULL;

			return retval;
		}

		/**
		 * Initialize a pin.
		 * @param b block this pin belongs to.
		 * @param name each pin has a descriptive name.
		 * @param dir direction of pin, one of DIR_INPUT or DIR_OUTPUT.
		 * @param bufsz preferred size of a managed buffer.
		 * @param bufcount preferred count of managed buffers.
		 * @return SUCCESS or FAILURE if out of memory.
		 */
		int32 init(Block* b, // parent block for this pin
	  		char* name, // descriptive name
			uint8 dir, // pin direction
			uint32 bufsz, // preferred buffer size
			uint32 bufcount   // preferred count of buffers
			);
	private:
		/* block this pin belongs to */
		Block* _block;

		/* pin's name */
		DataBuf _name;

		/* pin's absolute name (block.pin) */
		DataBuf _absname;

		/* table of peers this pin is connected to (key=AbsoluteName,val=ptr Peer) */
		SharedHash _peers;

		/* pin status */
		int32 _status;

		/* pin direction */
		uint8 _direction; 

		/* flag: buffer pool has been set */
		bool _bpSet;

		/* preferred buffer size */
		uint32 _pref_bufsz;

		/* preferred buffers count */
		int32 _pref_bufcount;

		/* real buffer size */
		uint32 _real_bufsz;

		/* real buffer count */
		int32 _real_bufcount;

		/* pointer to input buffer pool for this pin */
		BufferPool* _ibp;

		/* table of wires on which data flow (key=Peer->AbsoluteName,val=ptr Wire) */
		SharedHash _wires;

		/* managed data flavour */
		DataType _dt;

		/* data subtype */
		SubType _subtype;
	};
}

#endif
