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

#ifndef PVECTOR_UNSAFE_HPP
#define PVECTOR_UNSAFE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "databuf.hpp"
#include "comparator.hpp"

namespace uStreamLib {
	/**
	 * Dynamic vector of pointers. Use this vector to store
	 * generic pointers. This vector is not thread safe.
	 */
	class US_API_EXPORT PVectorUnsafe : public Object {
	public:
		/**
		 * Constructor.
		 */
		PVectorUnsafe(void);

		/**
		 * Destructor.
		 */
		virtual ~PVectorUnsafe(void);

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
		int32 add(void* ptr);

		/**
		 * Remove a pointer from the vector.
		 * @param idx index of the pointer to remove.
		 * @return SUCCESS or FAILURE if idx is out of range.
		 */
		int32 del(uint32 idx);

		/**
		 * Replace specified pointer.
		 * @param idx index of the pointer to remove.
		 * @return SUCCESS or FAILURE if idx is out of range.    
		 */
		int32 set(uint32 idx, void* ptr);

		/**
		 * Operator [] to get the ith pointer.
		 * @param idx index of the pointer to get.
		 */
		void* operator[](uint32 idx);

		/**
		 * Clear the vector.
		 */
		void clear(void);

		/**
		 * Get vector size.
		 */
		uint32 getSize(void);

		/**
		 * Get elements count.
		 */
		uint32 getCount(void);

		/**
		 * Get reallocation increment.
		 */
		uint32 getIncrement(void);

		/**
		 * Get comparator object.
		 */
		Comparator* getComparator(void)
		{
			return m_comp;
		}

		/**
		 * Set comparator object.
		 */
		void setComparator(Comparator* c)
		{
			m_comp = c;
		}

		/**
		 * Sort elements.
		 */
		void sort(void);
	private:
		/* array of void pointers */
		void** m_vpPtrs;

		/* actual size */
		uint32 m_uSize;

		/* elements count */
		uint32 m_uCount;

		/* realloc increment */
		uint32 m_uIncrement;

		/* pointer to comparator */
		Comparator* m_comp;

		/* method for sorting items */
		void _quickSort(int32 idx1, int32 idx2, Comparator* comp);
	};
}

#endif
