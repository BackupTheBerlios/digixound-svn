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

#ifndef PVECTOR_HPP
#define PVECTOR_HPP

#include "pvector_unsafe.hpp"
#include "mutex.hpp"

namespace uStreamLib {
	/**
	 * Dynamic vector of pointers. Use this vector to store
	 * generic pointers.
	 */
	class US_API_EXPORT PVector : public PVectorUnsafe, public Mutex {
	public:
		/**
		 * Constructor.
		 */
		PVector(void);

		/**
		 * Destructor.
		 */
		virtual ~PVector(void);

		/**
		 * Initialize a vector of pointer.
		 * @return SUCCESS or FAILURE if out of memory.
		 */
		int32 init(uint32 init_size = 128, uint32 incr = 16);

		/**
		 * Add a pointer to the vector.
		 * @param ptr the pointer to add.
		 * @return SUCCESS or FAILURE if vector cannot be expanded.
		 */
		int32 add(void* ptr)
		{
			MutexLocker ml(this); return PVectorUnsafe::add(ptr);
		}

		/**
		 * Remove a pointer from the vector.
		 * @param idx index of the pointer to remove.
		 * @return SUCCESS or FAILURE if idx is out of range.
		 */
		int32 del(uint32 idx)
		{
			MutexLocker ml(this); return PVectorUnsafe::del(idx);
		}

		/**
		 * Operator [] to get the ith pointer. Please note that
		 * you must lock the vector before using this operator
		 * and then unlock it when done.
		 * @param idx index of the pointer to get.
		 */
		void* operator[](uint32 idx)
		{
			MutexLocker ml(this); return (*(PVectorUnsafe *) this)[idx];
		}

		/**
		 * Clear the vector.
		 */
		void clear(void)
		{
			MutexLocker ml(this); PVectorUnsafe::clear();
		}

		/**
		 * Get vector size.
		 */
		uint32 getSize(void)
		{
			MutexLocker ml(this); return PVectorUnsafe::getSize();
		}

		/**
		 * Get elements count.
		 */
		uint32 getCount(void)
		{
			MutexLocker ml(this); return PVectorUnsafe::getCount();
		}

		/**
		 * Get reallocation increment.
		 */
		uint32 getIncrement(void)
		{
			MutexLocker ml(this); return PVectorUnsafe::getIncrement();
		}
	};
}

#endif
