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

#include "mutex.hpp"

/*
 * This class is a proxy for specific implementation.
 */

namespace uStreamLib {
	/*
	 * Implementation of mutex.
	 */

	Mutex::Mutex(void)
		: Object(UOSUTIL_RTTI_MUTEX)
	{
		_impl = new Impl_Mutex();
	}

	Mutex::~Mutex(void)
	{
		delete _impl;
		// DEBUG
                //UOSUTIL_DERR((stderr, "~Mutex()\n"));
	}

	/*
	 * Implementation of mutex locker.
	 */

	MutexLocker::MutexLocker(Mutex* mutex)
		: Object(UOSUTIL_RTTI_MUTEX_LOCKER), _mutex(mutex)
	{
		if (_mutex)
			_mutex->lock();
	}

	MutexLocker::~MutexLocker(void)
	{
		_mutex->unlock();
	}
}
