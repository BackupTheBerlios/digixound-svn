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

#include "queue.hpp"

namespace uStreamLib {
	Queue::Queue(void)
		: Object(UOSUTIL_RTTI_QUEUE), _item_size(0), _capacity(0)
	{
		// nothing to do
	}

	int32 Queue::init(char* name, uint32 max_item_size, int32 capacity)
	{
		int32 ret = 0;

		ret = _name.init(name);
		if (ret == FAILURE)
			return FAILURE;

		_item_size = max_item_size;
		_capacity = capacity;

		setOk(true);
		return SUCCESS;
	}

	Queue::~Queue(void)
	{
		// nothing to do
	}
}
