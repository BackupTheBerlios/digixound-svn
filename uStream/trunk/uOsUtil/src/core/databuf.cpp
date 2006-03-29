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
#include <ctype.h>

#include "databuf.hpp"
#include "memory.hpp"

namespace uStreamLib {
	DataBuf::DataBuf(void)
		: Object(UOSUTIL_RTTI_DATABUF), m_pbpParent(NULL), m_uBID(0),
		m_strBlock(NULL), m_uSize(0), m_uCount(0), m_uLimit(65536),
		m_bInUse(false), m_asStrategy(ALLOC_ONUSE)
	{
		// nothing to do
	}

	DataBuf::~DataBuf(void)
	{
		if (m_strBlock)
			::free(m_strBlock);
	}

	int32 DataBuf::init(uint32 size, uint32 bid, uint32 limit,
		AllocStrategy strategy, BufferPool* bp)
	{
		// setup strategy
		m_asStrategy = strategy;

		// check requested size (zero fails)
		if (!size)
			return FAILURE;

		// align memory size to 64 bit boundaries
		if (size % 8)
			m_uSize = ((size >> 3) + 1) << 3;
		else
			m_uSize = size;

		// allocate memory only if alloc on create
		if (strategy == ALLOC_ONCREATE) {
			m_strBlock = (char *) malloc(m_uSize);
			if (!m_strBlock)
				return FAILURE;
		} else {
			m_strBlock = NULL;
			m_uSize = 0;
		}

		// initialize members
		m_uBID = bid;
		m_uCount = 0;
		m_uLimit = limit;
		m_pbpParent = bp;
		m_bInUse = false;

		// check limit
		if (m_uSize > m_uLimit)
			m_uLimit = m_uSize;

		// ok
		setOk(true);
		return SUCCESS;
	}

	int32 DataBuf::init(char* string)
	{
		// memory size is enough for this string
		uint32 uStrLen = (uint32) strlen(string) + 1;

		// stretch to a larger size
		if (uStrLen <= 16)
			m_uSize = 32;
		else
			m_uSize = uStrLen;

		// align memory size to 64 bit boundaries
		if (m_uSize % 8)
			m_uSize = ((m_uSize >> 3) + 1) << 3;

		// allocate memory
		m_strBlock = (char *) malloc(m_uSize);
		if (!m_strBlock)
			return FAILURE;

		// initialize members
		m_uBID = 0;
		m_uCount = uStrLen;
		m_uLimit = m_uSize << 1;
		m_asStrategy = ALLOC_ONCREATE;
		m_bInUse = false;
		m_pbpParent = NULL;

		// copy string into buffer
		snprintf(m_strBlock, m_uSize, "%s", string);

		// ok
		setOk(true);
		return SUCCESS;
	}

	int32 DataBuf::init(DataBuf& db)
	{
		int32 ret = 0;

		// initialize buffer
		ret = init(db.getSize());
		if (ret == FAILURE)
			return FAILURE;

		// copy data
		copy(&db);

		// ok
		setOk(true);
		return SUCCESS;
	}

	void DataBuf::copy(char* data, uint32 size)
	{
		// copy of zero bytes not allowed
		if (!size)
			return;

		// allocate memory if strategy is alloc on use
		if (m_asStrategy == ALLOC_ONUSE && !m_strBlock) {
			// align memory size to 64 bit boundaries
			if (size % 8)
				m_uSize = ((size >> 3) + 1) << 3;
			else
				m_uSize = size;

			// allocate memory
			m_strBlock = (char *) malloc(m_uSize);
			if (!m_strBlock)
				return;
		}

		// check size
		uint32 sz = size < m_uSize ? size : m_uSize;
		if (!sz)
			return;

		// copy data
		Memory::memCopy(m_strBlock, data, sz);
		m_uCount = sz;
	}

	int32 DataBuf::xcopy(char* data, uint32 size)
	{
		// copy of zero bytes not allowed
		if (!size)
			return FAILURE;

		// allocate memory if strategy is alloc on use
		if (m_asStrategy == ALLOC_ONUSE && !m_strBlock) {
			// align memory size to 64 bit boundaries
			if (size % 8)
				m_uSize = ((size >> 3) + 1) << 3;
			else
				m_uSize = size;

			// allocate memory
			m_strBlock = (char *) malloc(m_uSize);
			if (!m_strBlock)
				return FAILURE;
		}

		// perform reallocation if needed
		if (m_uSize < size) {
			// align memory size to 64 bit boundaries
			if (size % 8)
				m_uSize = ((size >> 3) + 1) << 3;
			else
				m_uSize = size;

			// allocate memory
			m_strBlock = (char *) ::realloc(m_strBlock, m_uSize);
			if (!m_strBlock)
				return FAILURE;
		}

		// copy data
		Memory::memCopy(m_strBlock, data, size);
		m_uCount = size;

		// ok
		return SUCCESS;
	}

	int32 DataBuf::merge(char* data, uint32 size)
	{
		uint32 uRequiredSize = 0;

		// copy of zero bytes not allowed
		if (!size)
			return FAILURE;

		// allocate memory if strategy is alloc on use
		if (m_asStrategy == ALLOC_ONUSE && !m_strBlock) {
			// align memory size to 64 bit boundaries
			if (size % 8)
				m_uSize = ((size >> 3) + 1) << 3;
			else
				m_uSize = size;

			// allocate memory
			m_strBlock = (char *) malloc(m_uSize);
			if (!m_strBlock)
				return FAILURE;
		}

		// compute new size
		uRequiredSize = m_uCount + size;

		// stretch buffer if necessary
		if (m_uSize < uRequiredSize) {
			// align memory size to 64 bit boundaries
			if (size % 8)
				m_uSize = ((uRequiredSize >> 3) + 1) << 3;
			else
				m_uSize = uRequiredSize;

			// allocate memory
			m_strBlock = (char *) ::realloc(m_strBlock, m_uSize);
			if (!m_strBlock)
				return FAILURE;
		}

		// copy data
		Memory::memCopy(&m_strBlock[m_uCount], data, size);
		m_uCount = uRequiredSize;

		// ok
		return SUCCESS;
	}

	int32 DataBuf::realloc(uint32 size)
	{
		// realloc of zero bytes not allowed
		if (!size)
			return FAILURE;

		// allocate memory if strategy is alloc on use
		if (m_asStrategy == ALLOC_ONUSE && !m_strBlock) {
			// align memory size to 64 bit boundaries
			if (size % 8)
				m_uSize = ((size >> 3) + 1) << 3;
			else
				m_uSize = size;

			// allocate memory
			m_strBlock = (char *) ::malloc(m_uSize);
			if (!m_strBlock)
				return FAILURE;
		}

		// do nothing if memory size is ok
		if (size == m_uSize)
			return SUCCESS;

		// align memory size to 64 bit boundaries
		if (size % 8)
			m_uSize = ((size >> 3) + 1) << 3;
		else
			m_uSize = size;

		// allocate memory
		m_strBlock = (char *) ::realloc(m_strBlock, m_uSize);
		if (!m_strBlock)
			return FAILURE;

		// ok
		return SUCCESS;
	}

	void DataBuf::move(int32 to, int32 from, uint32 size)
	{
		register uint32 fps = from + size;
		register uint32 tps = to + size;

		// move of zero bytes not allowed
		if (!size)
			return;

		// check to and from against 0
		if (to < 0 || from < 0)
			return;

		// check to and from against size
		if ((uint32) to > m_uSize)
			return;
		if ((uint32) from > m_uSize)
			return;

		// check fps and tps against size
		if (fps > m_uSize)
			return;
		if (tps > m_uSize)
			return;

		// copy memory
		Memory::memCopy(m_strBlock + to, m_strBlock + from, size);
	}

	void DataBuf::moveOnStart(int32 from, uint32 size)
	{
		register uint32 fps = from + size;

		// move of zero bytes not allowed
		if (!size)
			return;

		// check from against 0
		if (from < 0)
			return;

		// check from against size
		if ((uint32) from > m_uSize)
			return;

		// check fps against size
		if (fps > m_uSize)
			return;

		// copy memory
		Memory::memCopy(m_strBlock, m_strBlock + from, size);
	}

	void DataBuf::set(int32 value)
	{
		register uint32 uWC = m_uSize >> 3;
		register int32 * dst = (int32 *) m_strBlock;

		// check memory pointer
		if (!m_strBlock)
			return;

		// copy 4 bytes at time
		while (uWC--) {
			*(dst++) = value;
			*(dst++) = value;
		}
	}

	void DataBuf::set(uint32 offset, int32 value)
	{
		// check memory pointer
		if (!m_strBlock)
			return;

		// check offset
		if (offset > m_uSize)
			return;

		// copy memory
		::memset(&m_strBlock[offset], value, m_uSize - offset);
	}

	void DataBuf::printHex(void)
	{
		uint32 i = 0;

		// check memory pointer
		if (!m_strBlock)
			return;

		// print hexadecimal values in buffer
		for (i = 0; i < m_uCount; i++) {
			if (isprint(m_strBlock[i]))
				printf("  %c ", m_strBlock[i]);
			else
				printf("%02X ", m_strBlock[i]);
		}
	}
}
