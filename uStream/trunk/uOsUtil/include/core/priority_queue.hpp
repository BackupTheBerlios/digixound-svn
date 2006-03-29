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

#ifndef PRIORITYQUEUE_HPP
#define PRIORITYQUEUE_HPP

#include "shared_queue.hpp"
#include "semaphore.hpp"
#include "mutex.hpp"

namespace uStreamLib {
	/**
	 * This is a priority queue that can be shared between a consumer
	 * and a producer thread. The queue provides N priority levels and
	 * and out of band (OOB) level which is the highest priority
	 * available. There is an anti-starvation mechanism to avoid
	 * starvation of lowest priority items. The get() is controlled by
	 * drawing rates (double) which tune the amount of items to retrieve
	 * for a specific priority.
	 * 
	 * The priority goes from 0 which is the max to N which is the min.
	 * The constants PRI_MAX and PRI_OOB can be used in put() to specify
	 * the max priority and the out of band priority respectively.
	 */
	class US_API_EXPORT PriorityQueue : public Queue {
	public:
		/**
		 * Priority queue constants.
		 */
		enum PriorityID { PRI_OOB = -1, PRI_MAX = 0 };

		/**
		 * Constructor.
		 */
		PriorityQueue(void);

		/**
		 * Destructor.
		 */
		virtual ~PriorityQueue(void);

		/**
		 * Initialize a priority queue.
		 */
		int32 init(char* name, uint32 max_item_size, int32 capacity,
			int32 priority_levels, int32 oob_capacity = 10);

		int32 get(char* item, uint32 size)
		{
			return _get(true, item, size);
		}

		int32 tryGet(char* item, uint32 size)
		{
			return _get(false, item, size);
		}

		int32 put(char* item, uint32 size, int32 priority);
		int32 put(char* item, uint32 size)
		{
			return put(item, size, 0);
		}

		int32 tryPut(char* item, uint32 size, int32 priority);
		int32 tryPut(char* item, uint32 size)
		{
			return tryPut(item, size, 0);
		}

		int32 getCount(void)
		{
			return _sem_item_count.getValue();
		}

		int32 getOOBCount(void)
		{
			return _oobq.getCount();
		}

		int32 getCountPerPriority(int32 priority);
		int32 setDrawingRate(int32 pri, double rate);
		double getDrawingRate(int32 pri);

	protected:
		/* array of shared queues */
		SharedQueue** _pq;

		/* out of band queue */
		SharedQueue _oobq;

		/* number of priority levels */
		int32 _priority_levels;

		/* current queue priority to fetch from */
		int32 _start;

		/* drawing rates (anti starvation) */
		double* _drawing_rate;

		/* drawing thresholds */
		int32* _threshold;

		/* fetched elements counters */
		int32* _items_drawn;

		/* insert semaphore */
		Semaphore _sem_item_count;

		/* fetch semaphore */
		Semaphore _sem_free_count;

		/* consumer protection mutex */
		Mutex _cmutex;

	private:
		// copy constructor not available
		PriorityQueue(PriorityQueue&)
		{
		}

		// method to get data
		int32 _get(bool blocking, char* data, uint32 size);
	};
}

#endif
