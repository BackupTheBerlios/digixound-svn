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

#include <stdlib.h>
#include <string.h>

#include "pstack.hpp"

namespace uStreamLib {
	PStack::PStack(void)
		: Object(UOSUTIL_RTTI_PSTACK), m_ptrVoidItems(NULL), m_iStackPointer(0),
		m_iSize(0)
	{
		// nothing to do here
	}

	PStack::~PStack(void)
	{
		// lock mutex
		MutexLocker ml(&m_mutex);

		// free memory
		if (m_ptrVoidItems) {
			::free(m_ptrVoidItems);
			m_ptrVoidItems = NULL;
		}
	}

	int32 PStack::init(uint32 size)
	{
		int32 ret = 0;

		// initialize mutex
		ret = m_mutex.init();
		if (ret == FAILURE)
			return FAILURE;

		// initialize stack
		m_ptrVoidItems = (void * *) malloc(size * sizeof(void *));
		if (!m_ptrVoidItems)
			return FAILURE;

		m_iSize = size;
		m_iStackPointer = -1;

		// zeroize memory
		memset(m_ptrVoidItems, 0, size * sizeof(void *));

		// ok
		setOk(true);
		return SUCCESS;
	}

	int32 PStack::getStackPointer(void)
	{
		MutexLocker ml(&m_mutex);
		return m_iStackPointer;
	}

	void* PStack::getTop(void)
	{
		MutexLocker ml(&m_mutex);

		if (m_ptrVoidItems && m_iStackPointer >= 0)
			return m_ptrVoidItems[m_iStackPointer];

		return NULL;
	}

	int32 PStack::push(void* item)
	{
		MutexLocker ml(&m_mutex);
		int32 iBound = m_iSize - 1;

		// check
		if (!m_ptrVoidItems)
			return NULL;

		// increment stack pointer
		m_iStackPointer++;
		if (m_iStackPointer > iBound) {
			m_iStackPointer = iBound;
			return FAILURE;
		}

		// store pointer
		m_ptrVoidItems[m_iStackPointer] = item;

		// ok
		return SUCCESS;
	}

	void* PStack::pop(void)
	{
		MutexLocker ml(&m_mutex);

		// check
		if (!m_ptrVoidItems)
			return NULL;

		if (m_iStackPointer < 0) {
			m_iStackPointer = -1;
			return NULL;
		}

		void* retval = NULL;
		retval = m_ptrVoidItems[m_iStackPointer];

		m_iStackPointer--;

		// ok
		return retval;
	}
}

