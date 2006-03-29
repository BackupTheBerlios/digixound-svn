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

#include "linux_semaphore.hpp"

namespace uStreamLib {
	Impl_Semaphore::Impl_Semaphore(void)
	{
		// nothing to do
	}

	Impl_Semaphore::~Impl_Semaphore(void)
	{
		sem_destroy(&_sem);
	}

	int32 Impl_Semaphore::init(uint32 init_value, bool process_shared)
	{
		int32 ret;

		ret = sem_init(&_sem, process_shared, init_value);
		if (ret != 0)
			return FAILURE;

		return SUCCESS;
	}

	int32 Impl_Semaphore::wait(void)
	{
		return sem_wait(&_sem);
	}

	int32 Impl_Semaphore::tryWait(void)
	{
		int32 ret;

		ret = sem_trywait(&_sem);
		if (ret < 0)
			return FAILURE;

		return SUCCESS;
	}

	int32 Impl_Semaphore::post(void)
	{
		int32 ret;

		ret = sem_post(&_sem);
		if (ret < 0)
			return FAILURE;

		return SUCCESS;
	}

	uint32 Impl_Semaphore::getValue(void)
	{
		int32 val;

		sem_getvalue(&_sem, &val);
		return (uint32) val;
	}

	char* Impl_Semaphore::getErrorString()
	{
		return strerror(errno);
	}
}
