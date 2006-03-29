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
#include <string.h>
#include <stdlib.h>

#include "simple_queue.hpp"
#include "memory.hpp"

namespace uStreamLib {
	SimpleQueue::SimpleQueue()
		: _item(NULL), _in(NULL), _out(NULL), _items_count(0), _free_count(0)
	{
		// nothing to do here
	}

	/*
	 * TODO: improve memory deallocation if malloc fails.
	 */
	int32 SimpleQueue::init(char* name, uint32 max_item_size, int32 capacity)
	{
		queue_item* cur = NULL;
		queue_item* pre = NULL;
		int32 i = 0, ret = 0;

		// initialize parent
		ret = Queue::init(name, max_item_size, capacity);
		if (ret == FAILURE)
			return FAILURE;

		// allocate memory for items
		_item = (queue_item *) malloc(sizeof(queue_item));
		if (!_item)
			return FAILURE;

		memset(_item, 0, sizeof(queue_item));

		_item->prev = NULL;
		_item->next = NULL;
		_item->data = (char *) malloc(max_item_size);
		if (!_item->data) {
			free(_item); return FAILURE;
		}
		_item->size = max_item_size;

		cur = _item;

		for (i = 1; i < capacity; i++) {
			cur->next = (queue_item *) malloc(sizeof(queue_item));
			if (!cur->next) {
				free(_item); return FAILURE;
			}

			pre = cur;
			cur = cur->next;

			cur->prev = pre;
			cur->data = (char *) malloc(max_item_size);
			if (!cur->data) {
				free(_item); return FAILURE;
			}

			memset(_item, 0, sizeof(max_item_size));

			cur->size = max_item_size;
			cur->next = NULL;
		}

		_in = _item;
		_out = _item;

		_items_count = 0;
		_free_count = capacity;

#ifdef UOSUTIL_QUEUE_DEBUG
		fprintf(stderr, "SimpleQueue: %s (capacity=%d, max_item_size=%d)\n",
			getName(), capacity, max_item_size);
#endif

		// ok
		setOk(true);
		return SUCCESS;
	}

	int32 SimpleQueue::put(char* itm, uint32 size)
	{
		if (_free_count <= 0)
			return FAILURE;
		if (!_in)
			_in = _item;

#ifdef UOSUTIL_QUEUE_DEBUG
		if (_in)
			fprintf(stderr, "%s:put(): [isz=%d,msz=%d,ic=%d/%d]\n", getName(),
				size, _item_size, _items_count + 1, _items_count + _free_count);
#endif

		_in->size = (size > _item_size) ? _item_size : size;
		Memory::memCopy(_in->data, itm, _in->size);
		_in = _in->next;

		_items_count++;
		_free_count--;
		return SUCCESS;
	}

	int32 SimpleQueue::get(char* itm, uint32 size)
	{
		if (_items_count <= 0)
			return FAILURE;
		if (!_out)
			_out = _item;

#ifdef UOSUTIL_QUEUE_DEBUG
		if (_out)
			fprintf(stderr, "%s:get():  [isz=%d,msz=%d,ic=%d/%d]\n",
				getName(), size, _item_size, _items_count - 1,
				_items_count + _free_count);
#endif

		Memory::memCopy(itm, _out->data,
			(size < _out->size) ? size : _out->size);
		_out = _out->next;

		_items_count--;
		_free_count++;
		return SUCCESS;
	}

	SimpleQueue::~SimpleQueue(void)
	{
		queue_item* cur;
		queue_item* pre;

		if (!_item)
			return;

		cur = _item;
		while (cur) {
			pre = cur;
			cur = cur->next;

			free(pre->data);
			free(pre);
		}
	}
}
