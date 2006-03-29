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

#ifndef WIRE_HPP
#define WIRE_HPP

#include "bufferpool.hpp"
#include "sharedvars.hpp"
#include "constants.hpp"
#include "types.hpp"

namespace uStreamLib {
	/*
	 * Declare a Pin and a Block.
	 */
	class Pin;
	class Block;

	/**
	 * This class is a Wire.
	 * Two Pins must be connected using a Wire. The Wire analyzes
	 * pins' preferred buffer sizes/counts and allocates one or
	 * two buffer pools to accomplish each pin requirement. A Wire
	 * can be unidirectional or bidirectional, ie 1 or 2 buffer pools.
	 */
	class US_EXPORT Wire : public Object {
	public:
		/**
		 * Direction of a Wire.
		 * A Wire can be point to point or point to multipoint
		 * and manage unidirectional/bidirectional transfers.
		 */
		enum { /** the wire is unidirectional */
		UNIDIRECTIONAL = 0, /** the wire is bidirectional */
		BIDIRECTIONAL = 1 };

		/**
		 * Constructor.
		 */
		Wire(void);

		/**
		 * Destructor.
		 * Use the destructor (delete a wire object) to disconnect
		 * two pins.
		 */
		virtual ~Wire(void);

		/**
		 * Initialize a wire.
		 * @param type one of UNIDRECTIONAL, BIDIRECTIONAL.
		 */
		int32 init(int32 type);

		/**
		 * Connect two pins. If one of the two pins is already connected
		 * to another pin using another Wire, this method shares the
		 * same buffer pool of connected pin. This behaviour allows point
		 * to multipoint connections.
		 * If unidirectional, data flows from p1 to p2.
		 * @param b1 the block whose pin we wish to connect to.
		 * @param p1 the first pin.
		 * @param b2 the block whose pin we wish to connect to.
		 * @param p2 the second pin.
		 * @return SUCCESS or FAILURE if memory cannot be allocated.
		 */
		virtual int32 connect(Block* b1, Pin* p1, Block* b2, Pin* p2);

		/**
		 * Get connected pins.
		 * @param which one of 1 or 2.
		 * @return a pin or NULL if not connected.
		 */
		Pin* getPin(int32 which)
		{
			return (which <= 1 ? _p1 : _p2);
		}

		/**
		 * Get buffer pools
		 * @param which one of 1 or 2.
		 * @return a buffer pool or NULL if not connected.
		 */
		BufferPool* getBufferPool(int32 which)
		{
			return (which <= 1 ? _bp1 : _bp2);
		}

		/**
		 * Get error string.
		 * @return a string.
		 */
		char* getErrorString(void)
		{
			return _error_string.get();
		}
	private:
		/*
		 * Other constructors.
		 */
		Wire(Wire&)
			: Object(UOSUTIL_RTTI_WIRE)
		{
		}

		/* type of the wire */
		int32 _type;

		/* buffer pool for pin 1 */
		BufferPool* _bp1;

		/* buffer pool for pin 2 */
		BufferPool* _bp2;

		/* connected pin 1 */
		Pin* _p1;

		/* connected pin 2 */
		Pin* _p2;

		/* buffer for error string */
		SharedString _error_string;

		/* flag: multipoint connection on bp1 */
		bool _multipoint_on_bp1;

		/* flag: multipoint connection on bp2 */
		bool _multipoint_on_bp2;

		/* allocate buffers */
		int32 _allocate(void);
	};
}

#endif
