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

#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include "object.hpp"

/*
 * Here, we choose the right implementation using
 * conditional compilation.
 */

#if defined(_WIN32) || defined(WIN32)
#include "win32_semaphore.hpp"
#else
#include "linux_semaphore.hpp"
#endif

namespace uStreamLib {
	/**
	 * This is a semaphore.
	 */
	class US_API_EXPORT Semaphore : public Object {
	public:
		/**
		 * Constructor.
		 */
		Semaphore(void);

		/**
		 * Destructor.
		 */
		virtual ~Semaphore(void);

		/**
		 * Initialize a semaphore object.
		 * @param init_value start value for this semaphore.
		 * @param process_shared true or false to share this
		 * semaphore among processes (this feature is not implemented).
		 * @return SUCCESS or FAILURE.
		 */
		int32 init(uint32 init_value, bool process_shared = false)
		{
			int32 ret = _impl->init(init_value, process_shared);
			if (ret == FAILURE)
				return FAILURE;

			setOk(true);
			return SUCCESS;
		}

		/**
		 * Decrease semaphore.
		 * This method will block if semaphore's value is lesser
		 * or equal to 0.
		 */
		int32 wait(void)
		{
			return _impl->wait();
		}

		/**
		 * Decrease semaphore.
		 * This method will return FAILURE if semaphore's value is lesser
		 * or equal to 0.
		 */
		int32 tryWait(void)
		{
			return _impl->tryWait();
		}

		/**
		 * Increase semaphore.
		 */
		int32 post(void)
		{
			return _impl->post();
		}

		/**
		 * Get semaphore's value.
		 */
		uint32 getValue(void)
		{
			return _impl->getValue();
		}

		/**
		 * Get system specific error string if an error occurred.
		 */
		char* getErrorString()
		{
			return _impl->getErrorString();
		}
		
	private:
		/* specific implementation */
		Impl_Semaphore* _impl;
	};
}

#endif
