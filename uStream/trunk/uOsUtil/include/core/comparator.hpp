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

#ifndef COMPARATOR_HPP
#define COMPARATOR_HPP

#include "databuf.hpp"

namespace uStreamLib {
	/**
	 * Class that abstracts a search criteria for objects.
	 */
	class US_API_EXPORT Comparator : public Object {
	public:
		/**
		 * Constructor.
		 */
		Comparator(void);

		/**
		 * Destructor.
		 */
		virtual ~Comparator(void);

		/**
		 * Override this method to provide your own compare code.
		 * @param a pointer to first data buffer.
		 * @param a_sz size of first data buffer.
		 * @param b pointer to second data buffer.
		 * @param b_sz size of second data buffer.
		 * @param return -1 if a < b, 0 if a = b, 1 if a > b.
		 */
		virtual int32 compare(void* a, uint32 a_sz, void* b, uint32 b_sz) = 0;
	};

	class US_API_EXPORT MemoryComparator : public Comparator {
	public:
		virtual int32 compare(void* a, uint32 a_sz, void* b, uint32 b_sz);
	};

	class US_API_EXPORT StringComparator : public Comparator {
	public:
		virtual int32 compare(void* a, uint32 a_sz, void* b, uint32 b_sz);
	};

	class US_API_EXPORT IntegerComparator : public Comparator {
	public:
		virtual int32 compare(void* a, uint32 a_sz, void* b, uint32 b_sz);
	};
}

#endif
