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

#include "win32_semaphore.hpp"

namespace uStreamLib {
	Impl_Semaphore::Impl_Semaphore(void)
		: _sem(NULL)
	{
		// nothing to do
	}

	Impl_Semaphore::~Impl_Semaphore(void)
	{
		if (_sem)
			::CloseHandle(_sem);
	}

	int32 Impl_Semaphore::init(uint32 init_value, bool)
	{
		int32 max_val = 0x7fffffff;

		_sem = ::CreateSemaphore(NULL, init_value, max_val, NULL);
		if (!_sem)
			return FAILURE;

		_value = init_value;
		return SUCCESS;
	}

	int32 Impl_Semaphore::wait(void)
	{
		DWORD ret = WAIT_FAILED;

		ret = ::WaitForSingleObject(_sem, INFINITE);
		if (ret == WAIT_OBJECT_0)
			return SUCCESS;

		return FAILURE;
	}

	int32 Impl_Semaphore::tryWait(void)
	{
		DWORD ret = WAIT_FAILED;

		ret = ::WaitForSingleObject(_sem, 0);
		if (ret == WAIT_OBJECT_0)
			return SUCCESS;

		return FAILURE;
	}

	int32 Impl_Semaphore::post(void)
	{
		BOOL ret;

		ret = ::ReleaseSemaphore(_sem, 1, (LPLONG) & _value);
		if (!ret)
			return FAILURE;

		return SUCCESS;
	}

	uint32 Impl_Semaphore::getValue(void)
	{
		return (uint32) _value;
	}

	char* Impl_Semaphore::getErrorString()
	{
		return "NOT_IMPLEMENTED";
	}
}
