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

#include <stdio.h>
#include <malloc.h>

#include "priority_queue.hpp"

namespace uStreamLib {
	PriorityQueue::PriorityQueue(void)
		: _pq(NULL), _priority_levels(0), _start(0), _drawing_rate(NULL),
		_threshold(NULL), _items_drawn(NULL)
	{
		Queue::setClassID(UOSUTIL_RTTI_PRIORITY_QUEUE);
	}

	PriorityQueue::~PriorityQueue(void)
	{
		int32 i;

		for (i = 0; i < _priority_levels; i++) {
			if (_pq[i])
				delete _pq[i];
		}

		if (_pq)
			delete[] _pq;
		if (_drawing_rate)
			delete[] _drawing_rate;
		if (_threshold)
			delete[] _threshold;
		if (_items_drawn)
			delete[] _items_drawn;
	}

	/*
	 * TODO: improve memory deallocation if malloc fails.
	 */
	int32 PriorityQueue::init(char* name, uint32 max_item_size,
		int32 capacity, int32 priority_levels, int32 oob_capacity)
	{
		int32 ret = 0, i = 0, sqcap = 0;
		char tmp[4096];

		/* initialize parent */
		ret = Queue::init(name, max_item_size, capacity + oob_capacity);
		if (ret == FAILURE)
			return FAILURE;

		/* malloc queues array */
		_pq = new SharedQueue * [priority_levels];
		if (!_pq)
			return FAILURE;

		/* compute queue aggregate capacity */
		sqcap = (int32) ((double) capacity / (double) priority_levels);

		/* create each queue (each capacity is 1/priority_levels of cap) */
		for (i = 0; i < priority_levels; i++) {
			snprintf(tmp, sizeof(tmp), "%s[%d]", getName(), i);

			_pq[i] = new SharedQueue();
			if (_pq[i]) {
				ret = _pq[i]->init(tmp, max_item_size, sqcap);
				if (ret == FAILURE) {
					delete _pq[i]; _pq[i] = NULL; return FAILURE;
				}
			}
		}

		/* create out of band queue */
		snprintf(tmp, sizeof(tmp), "%s[OOB]", getName());
		ret = _oobq.init(tmp, max_item_size, oob_capacity);
		if (ret == FAILURE)
			return FAILURE;

		/* create fetch counters array */
		_items_drawn = new int32[priority_levels];
		if (!_items_drawn)
			return FAILURE;

		/* create fetch coefficents array */
		_drawing_rate = new double[priority_levels];
		if (!_drawing_rate)
			return FAILURE;

		/* create thresholds array */
		_threshold = new int32[priority_levels];
		if (!_threshold)
			return FAILURE;

		/* setup status */
		_priority_levels = priority_levels;
		_start = 0;

		/* compute drawing rates */
		for (i = 0; i < priority_levels; i++) {
			_drawing_rate[i] = .5 / ((double) i + 1.0);
			_threshold[i] = (int32) (_drawing_rate[i] * (double) sqcap);
			_items_drawn[i] = 0;
		}

		/* setup semaphores */
		ret = _sem_item_count.init(0);
		if (ret == FAILURE)
			return FAILURE;

		ret = _sem_free_count.init(_capacity);
		if (ret == FAILURE)
			return FAILURE;

		/* setup mutexes */
		ret = _cmutex.init();
		if (ret == FAILURE)
			return FAILURE;

		// ok
		setOk(true);
		return SUCCESS;
	}

	int32 PriorityQueue::_get(bool blocking, char* item, uint32 size)
	{
		int32 ret = 0;
		int32 i = 0, j = 0;

		// wait for semaphore or return
		if (blocking)
			_sem_item_count.wait();
		else if (_sem_item_count.tryWait() == FAILURE)
			return FAILURE;

		// check out of band queue (and post semaphore if ok)
		ret = _oobq.tryGet(item, size);
		if (ret == SUCCESS) {
			_sem_free_count.post(); return SUCCESS;
		}

		// lock consumer mutex
		MutexLocker ml(&_cmutex);

		// check other queues
		j = _start;
		i = 0;

		do {
			// check on queue with priority j
			if (_pq && _pq[j]) {
				ret = _pq[j]->tryGet(item, size);
				if (ret == SUCCESS) {
					// do anti starvation checking
					_items_drawn[j] += 1;
					if (_items_drawn[j] >= _threshold[j]) {
						_start += 1;
						if (_start >= _priority_levels)
							_start = 0;

						_items_drawn[j] = 0;
					}

					// signal data fetched
					_sem_free_count.post();

					// return data found
					return SUCCESS;
				}

				j += 1;
				if (j >= _priority_levels)
					j = 0;
			}
		} while (++i < _priority_levels);

		return FAILURE;
	}

	int32 PriorityQueue::put(char* item, uint32 size, int32 priority)
	{
		if (priority > _priority_levels)
			return FAILURE;

		_sem_free_count.wait();

		if (priority <= PRI_OOB) {
			_oobq.put(item, size);
		} else {
			if (_pq && _pq[priority]) {
				_pq[priority]->put(item, size);
			}
		}

		_sem_item_count.post();
		return SUCCESS;
	}

	int32 PriorityQueue::tryPut(char* item, uint32 size, int32 priority)
	{
		if (priority > _priority_levels)
			return FAILURE;

		if (_sem_free_count.tryWait() == FAILURE)
			return FAILURE;

		if (priority <= PRI_OOB) {
			_oobq.put(item, size);
		} else {
			if (_pq && _pq[priority]) {
				int32 ret = _pq[priority]->tryPut(item, size);
				if (ret == FAILURE)
					return FAILURE;
			}
		}

		_sem_item_count.post();
		return SUCCESS;
	}

	int32 PriorityQueue::getCountPerPriority(int32 priority)
	{
		if (priority >= 0 && priority < _priority_levels) {
			if (_pq && _pq[priority]) {
				return _pq[priority]->getCount();
			}
		}

		return FAILURE;
	}

	int32 PriorityQueue::setDrawingRate(int32 pri, double rate)
	{
		if (pri >= 0 && pri < _priority_levels) {
			if (_drawing_rate && _threshold) {
				_drawing_rate[pri] = rate;
				_threshold[pri] = (int32)
					(_drawing_rate[pri] * (double) _capacity);

				return SUCCESS;
			}
		}

		return FAILURE;
	}

	double PriorityQueue::getDrawingRate(int32 pri)
	{
		if (pri >= 0 && pri < _priority_levels) {
			if (_drawing_rate) {
				return _drawing_rate[pri];
			}
		}

		return 0.0;
	}
}
