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

#ifndef SHAREDQUEUE_HPP
#define SHAREDQUEUE_HPP

#include "simple_queue.hpp"
#include "semaphore.hpp"
#include "mutex.hpp"

namespace uStreamLib {
	/**
	 * This is a multi_producer - multi_consumer queue. It can
	 * be shared between two or more threads.
	 */
	class US_API_EXPORT SharedQueue : public SimpleQueue {
	public:
		/**
		 * Constructor.
		 */
		SharedQueue(void);

		/**
		 * Destructor.
		 */
		virtual ~SharedQueue(void);

		/**
		 * Create a shared queue.
		 * This queue can be shared among threads.
		 * @param name queue's name.
		 * @param max_item_size max item size.
		 * @param capacity capacity.
		 * @return SUCCESS or FAILURE.
		 */
		int32 init(char* name, uint32 max_item_size, int32 capacity);

		/**
		 * Put an item in the queue.
		 * This method blocks until the item is put into the queue.
		 * @param item something to put in the queue (data must be casted
		 * to char* here).
		 * @param size size of the item.
		 * @return SUCCESS or FAILURE if an error occurred.
		 */
		int32 put(char* item, uint32 size);

		/**
		 * Put an item in the queue.
		 * This method tries to put an item into the queue.
		 * @param item something to put in the queue (data must be casted
		 * to char* here).
		 * @param size size of the item.
		 * @return SUCCESS or FAILURE if the queue is full.
		 */
		int32 tryPut(char* item, uint32 size);

		/**
		 * Get next item from the queue.
		 * This method blocks until an item is available in the queue.
		 * @param item pointer to a buffer to store the item into.
		 * @param size size of the buffer.
		 * @return SUCCESS or FAILURE if an error occurred.
		 */
		int32 get(char* item, uint32 size);

		/**
		 * Get next item from the queue.
		 * This method check for next item in the queue. If an item is not
		 * available, ie the queue is empty, then it returns FAILURE.
		 * @param item pointer to a buffer to store the item into.
		 * @param size size of the buffer.
		 * @return SUCCESS or FAILURE if the queue is empty.
		 */
		int32 tryGet(char* item, uint32 size);

		/**
		 * Get current items count.
		 * @return current items count.
		 */
		int32 getCount(void);
	
	private:
		// no copy constructors
		SharedQueue(SharedQueue&)
		{
		}

		// count of items in the queue
		Semaphore _sem_item_count;

		// count of free slots
		Semaphore _sem_free_count;

		// multiple producer mutex
		Mutex _pmutex;

		// multiple consumer mutex
		Mutex _cmutex;
	};
}

#endif
