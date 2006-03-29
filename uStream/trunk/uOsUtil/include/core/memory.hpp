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

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "typedefs.hpp"

namespace uStreamLib {
	/**
	 * Optimized memory methods.
	 */
	class US_API_EXPORT Memory {
	public:
		/**
		 * Memory copy function pointer type.
		 */
		typedef void*(*MemFunc)(void* dst, const void* src, size_t size);

		/**
		 * Perform benchmark to detect best memcpy method.
		 */
		static void benchmark(void);

		/**
		 * Perform benchmark to detect best memcpy method.
		 */
		static void benchmark(uint32 size, uint32 i_count);

		/**
		 * Copy memory from source buffer to destination buffer.
		 * @param dst pointer to destination buffer.
		 * @param src pointer to source buffer.
		 * @param size count of bytes to copy.
		 * @return a pointer to destination buffer.
		 */
		static void* memCopy(void* dst, void* src, size_t size)
		{
			void* retval = NULL;

			if (size <= 1000000)
				retval = _bestFuncSmall(dst, src, size);
			else if (size > 1000000 && size <= 2000000)
				retval = _bestFunc1M(dst, src, size);
			else if (size >= 2000000)
				retval = _bestFunc2M(dst, src, size);

			return retval;
		}
	private:
		/* registered copy function names count */
		enum { _MEMORY_FNAME_SIZE = 50 };

		/* pointer to best copy function for < 1M */
		static MemFunc _bestFuncSmall;

		/* pointer to best copy function for 1M : 2M */
		static MemFunc _bestFunc1M;

		/* pointer to best copy function for > 2M */
		static MemFunc _bestFunc2M;

		/* best function's name */
		static char _bestFuncName[_MEMORY_FNAME_SIZE];
	};
}

#endif
