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

#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "databuf.hpp"

namespace uStreamLib {
	/**
	 * Abstract class for queue.
	 */
	class US_API_EXPORT Queue : public Object {
	public:
		/**
		 * Constructor.
		 */
		Queue(void);

		/**
		 * Destructor.
		 */
		virtual ~Queue(void);

		/**
		 * Create a queue.
		 * @param name queue's name.
		 * @param max_item_size max item size.
		 * @param capacity capacity.
		 * @return SUCCESS or FAILURE.
		 */
		int32 init(char* name, uint32 max_item_size, int32 capacity);

		/**
		 * Get next item from the queue.
		 * This method blocks until an item is available in the queue.
		 * @param item pointer to a buffer to store the item into.
		 * @param size size of the buffer.
		 * @return SUCCESS or FAILURE if an error occurred.
		 */
		virtual int32 get(char* item, uint32 size) = 0;

		/**
		 * Get next item from the queue.
		 * This method check for next item in the queue. If an item is not
		 * available, ie the queue is empty, then it returns FAILURE.
		 * @param item pointer to a buffer to store the item into.
		 * @param size size of the buffer.
		 * @return SUCCESS or FAILURE if the queue is empty.
		 */
		virtual int32 tryGet(char* item, uint32 size) = 0;

		/**
		 * Put an item in the queue.
		 * This method blocks until the item is put into the queue.
		 * @param item something to put in the queue (data must be casted
		 * to char* here).
		 * @param size size of the item.
		 * @return SUCCESS or FAILURE if an error occurred.
		 */
		virtual int32 put(char* item, uint32 size) = 0;

		/**
		 * Put an item in the queue.
		 * This method tries to put an item into the queue.
		 * @param item something to put in the queue (data must be casted
		 * to char* here).
		 * @param size size of the item.
		 * @return SUCCESS or FAILURE if the queue is full.
		 */
		virtual int32 tryPut(char* item, uint32 size) = 0;

		/**
		 * Get current items count.
		 * @return current items count.
		 */
		virtual int32 getCount(void) = 0;

		/**
		 * Get capacity.
		 */
		int32 getCapacity(void)
		{
			return _capacity;
		}

		/**
		 * Get max item size.
		 */
		uint32 getItemSize(void)
		{
			return _item_size;
		}

		/**
		 * Get queue's name.
		 */
		char* getName(void)
		{
			return _name.toString();
		}

		/* a queue slot */
		struct queue_item {
			/* data size */
			uint32 size;

			/* buffer to store the data into */
			char* data;

			/* ptr to next queue item */
			struct queue_item* next;

			/* ptr to previous item */
			struct queue_item* prev;
		};

	protected:
		/* queue's name */
		DataBuf _name;

		/* max element size */
		uint32 _item_size;

		/* queue capacity */
		int32 _capacity;

	private:
		/* no copy constructor */
		Queue(Queue& q)
			: Object(q)
		{
		}
	};
}

#endif
