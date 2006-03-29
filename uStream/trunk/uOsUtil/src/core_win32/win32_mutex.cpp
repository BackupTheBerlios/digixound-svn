/*
  uSTREAM LIGHT-WEIGHT STREAMING ARCHITECTURE
  Copyright (C) 2005 Luis Serrano (luis@kontrol-dj.com) 
  
  Based on DANUBIO STREAMING ARCHITECTURE by Michele Iacobellis (m.iacobellis@nexotech.it)
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
*/

#include "win32_mutex.hpp"

namespace uStreamLib {
	Impl_Mutex::Impl_Mutex(void)
		: _mutex(NULL)
	{
		// nothing to do
	}

	Impl_Mutex::~Impl_Mutex(void)
	{
		if (_mutex)
			::CloseHandle(_mutex);
	}

	int32 Impl_Mutex::init(void)
	{
		_mutex = ::CreateMutex(NULL, FALSE, NULL);
		if (_mutex == NULL)
			return FAILURE;

		return SUCCESS;
	}

	int32 Impl_Mutex::lock(void)
	{
		DWORD ret = WAIT_FAILED;

		ret = ::WaitForSingleObject(_mutex, INFINITE);
		if (ret == WAIT_OBJECT_0)
			return SUCCESS;

		return FAILURE;
	}

	int32 Impl_Mutex::tryLock(void)
	{
		DWORD ret = WAIT_FAILED;

		ret = ::WaitForSingleObject(_mutex, 0);
		if (ret == WAIT_OBJECT_0)
			return SUCCESS;

		return FAILURE;
	}

	int32 Impl_Mutex::unlock(void)
	{
		BOOL ret;

		ret = ::ReleaseMutex(_mutex);
		if (!ret)
			return FAILURE;

		return SUCCESS;
	}

	char* Impl_Mutex::getErrorString(void)
	{
		return "NOT_IMPLEMENTED";
	}
}
