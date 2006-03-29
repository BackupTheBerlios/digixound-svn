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

#ifndef MUTEX_HPP
#define MUTEX_HPP

#include "object.hpp"

/*
 * Here, we choose the right implementation using
 * conditional compilation.
 */

#if defined(_WIN32) || defined(WIN32)
#include "win32_mutex.hpp"
#else
#include "linux_mutex.hpp"
#endif

namespace uStreamLib {
	/**
	 * This is a mutex.
	 */
	class US_API_EXPORT Mutex : public Object {
	public:
		/**
		 * Constructor.
		 */
		Mutex(void);

		/**
		 * Destructor.
		 */
		virtual ~Mutex(void);

		/**
		 * Initialize the mutex.
		 * @return SUCCESS or FAILURE if the mutex cannot
		 * be created.
		 */
		int32 init(void)
		{
			int32 ret = _impl->init();
			if (ret == FAILURE)
				return FAILURE;

			setOk(true);
			return SUCCESS;
		}

		/**
		 * Lock the mutex.
		 * This method blocks until the mutex is locked.
		 * @return SUCCESS or FAILURE.
		 */
		int32 lock(void)
		{
			return _impl->lock();
		}

		/**
		 * Try to lock the mutex.
		 * This method check if I can lock the mutex. If not, then
		 * it returns FAILURE.
		 * @return SUCCESS or FAILURE if the mutex cannot be locked,
		 * ie is locked my another thread.
		 */
		int32 tryLock(void)
		{
			return _impl->tryLock();
		}

		/**
		 * Unlock the mutex.
		 * @return SUCCESS or FAILURE.
		 */
		int32 unlock(void)
		{
			return _impl->unlock();
		}

		/**
		 * Get system specific error string.
		 */
		char* getErrorString(void)
		{
			return _impl->getErrorString();
		}
		
	private:
		/* specific implementation object */
		Impl_Mutex* _impl;
	};

	/**
	 * Utility class for automatically lock/unlock mutexes.
	 */
	class US_API_EXPORT MutexLocker : public Object {
	public:
		/**
		 * Create a mutex locker.
		 * @param mutex pointer to an initialized mutex.
		 */
		MutexLocker(Mutex* mutex);

		/**
		 * Destructor.
		 * Unlock the mutex.
		 */
		~MutexLocker(void);
	private:
		/* pointer to initialized mutex */
		Mutex* _mutex;
	};
}

#endif
