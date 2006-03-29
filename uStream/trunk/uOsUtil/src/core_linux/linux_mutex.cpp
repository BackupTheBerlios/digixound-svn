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

#include <errno.h>
#include <string.h>

#include "linux_mutex.hpp"

namespace uStreamLib {
	Impl_Mutex::Impl_Mutex(void)
	{
		// nothing to do
	}

	Impl_Mutex::~Impl_Mutex(void)
	{
		pthread_mutex_destroy(&_mutex);
	}

	int32 Impl_Mutex::init(void)
	{
		return pthread_mutex_init(&_mutex, NULL);
	}

	int32 Impl_Mutex::lock(void)
	{
		return pthread_mutex_lock(&_mutex);
	}

	int32 Impl_Mutex::tryLock(void)
	{
		int32 ret;

		ret = pthread_mutex_trylock(&_mutex);
		if (ret == EBUSY)
			return FAILURE;

		return SUCCESS;
	}

	int32 Impl_Mutex::unlock(void)
	{
		return pthread_mutex_unlock(&_mutex);
	}

	char* Impl_Mutex::getErrorString(void)
	{
		return strerror(errno);
	}
}
