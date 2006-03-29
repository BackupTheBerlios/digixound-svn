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
#include <stdlib.h>
#include <string.h>

#include "enum.hpp"

namespace uStreamLib {
	Enumeration::Enumeration(void)
		: Object(UOSUTIL_RTTI_ENUMERATION), m_peiItems(NULL), m_uCount(0),
		m_uSize(0), m_uCurItem(0), m_peiCur(NULL)
	{
		// nothing to do
	}

	Enumeration::~Enumeration(void)
	{
		if (m_peiItems)
			::free(m_peiItems);
	}

	int32 Enumeration::init(uint32 size)
	{
		uint32 sz = sizeof(EnumItem) * size;
		m_peiItems = NULL;

		if (sz) {
			m_peiItems = (EnumItem *) malloc(sz);
			if (!m_peiItems)
				return FAILURE;

			memset(m_peiItems, 0, sz);
		}

		m_uSize = size;
		m_uCount = 0;
		m_uCurItem = 0;
		m_peiCur = m_peiItems;

		setOk(true);
		return SUCCESS;
	}

	void* Enumeration::nextElement(uint32* item_size)
	{
		void* retval = NULL;

		*item_size = m_peiCur->size;
		retval = m_peiCur->value;

		m_uCurItem += 1;
		m_peiCur += 1;

		return retval;
	}

	void Enumeration::addElement(void* item, uint32 size)
	{
		uint32 new_size = m_uSize + m_uSize / 4;
		uint32 memsize = new_size * sizeof(EnumItem);

		if (m_uCount >= m_uSize) {
			m_peiItems = (EnumItem *) ::realloc(m_peiItems, memsize);
			if (!m_peiItems)
				return;

			m_uSize = new_size;
		}

		m_peiItems[m_uCurItem].value = item;
		m_peiItems[m_uCurItem].size = size;

		m_uCount += 1;
		m_uCurItem += 1;
		m_peiCur += 1;
	}
}
