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

#include "shared_queue.hpp"

namespace uStreamLib {
	SharedQueue::SharedQueue(void)
	{
		setClassID(UOSUTIL_RTTI_SHARED_QUEUE);
	}

	int32 SharedQueue::init(char* name, uint32 max_item_size, int32 capacity)
	{
		int32 ret = 0;

		// initialize parent
		ret = SimpleQueue::init(name, max_item_size, capacity);
		if (ret == FAILURE)
			return FAILURE;

		// create semaphores
		ret = _sem_item_count.init(0);
		if (ret == FAILURE)
			return FAILURE;

		ret = _sem_free_count.init(capacity);
		if (ret == FAILURE)
			return FAILURE;

		// create mutexes
		ret = _pmutex.init();
		if (ret == FAILURE)
			return FAILURE;

		ret = _cmutex.init();
		if (ret == FAILURE)
			return FAILURE;

		// ok
		setOk(true);
		return SUCCESS;
	}

	SharedQueue::~SharedQueue(void)
	{
		// nothing to do
	}

	int32 SharedQueue::put(char* item, uint32 size)
	{
		int32 ret;

		_sem_free_count.wait();

		_pmutex.lock();
		ret = SimpleQueue::put(item, size);
		_pmutex.unlock();

		_sem_item_count.post();

		return ret;
	}

	int32 SharedQueue::tryPut(char* item, uint32 size)
	{
		int32 ret;

		if (_sem_free_count.tryWait() == FAILURE)
			return FAILURE;

		ret = _pmutex.tryLock();
		if (ret == FAILURE) {
			_sem_free_count.post(); return FAILURE;
		}

		ret = SimpleQueue::put(item, size);
		_pmutex.unlock();

		_sem_item_count.post();

		return ret;
	}

	int32 SharedQueue::get(char* item, uint32 size)
	{
		int32 ret;

		_sem_item_count.wait();

		_cmutex.lock();
		ret = SimpleQueue::get(item, size);
		_cmutex.unlock();

		_sem_free_count.post();

		return ret;
	}

	int32 SharedQueue::tryGet(char* item, uint32 size)
	{
		int32 ret;

		if (_sem_item_count.tryWait() == FAILURE)
			return FAILURE;

		ret = _cmutex.tryLock();
		if (ret == FAILURE) {
			_sem_item_count.post(); return FAILURE;
		}

		ret = SimpleQueue::get(item, size);
		_cmutex.unlock();

		_sem_free_count.post();

		return ret;
	}

	int32 SharedQueue::getCount(void)
	{
		return _sem_item_count.getValue();
	}
}
