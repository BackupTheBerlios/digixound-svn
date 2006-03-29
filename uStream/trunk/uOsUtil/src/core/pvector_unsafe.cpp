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

#include "pvector_unsafe.hpp"

namespace uStreamLib {
	PVectorUnsafe::PVectorUnsafe(void)
		: Object(UOSUTIL_RTTI_PVECTOR_UNSAFE), m_vpPtrs(NULL), m_uSize(0),
		m_uCount(0), m_uIncrement(0), m_comp(NULL)
	{
		// nothing to do
	}

	PVectorUnsafe::~PVectorUnsafe(void)
	{
		// free memory
		if (m_vpPtrs) {
			::free(m_vpPtrs);
			m_vpPtrs = NULL;
		}

		m_uCount = 0;
		m_uSize = 0;
		m_uIncrement = 0;
	}

	int32 PVectorUnsafe::init(uint32 init_size, uint32 incr)
	{
		// initialize pointers array
		m_vpPtrs = (void * *) ::malloc(init_size * sizeof(void *));
		if (!m_vpPtrs)
			return FAILURE;

		// zeroize buffer
		::memset(m_vpPtrs, 0, init_size * sizeof(void *));

		// initialize members
		m_uCount = 0;
		m_uSize = init_size;
		m_uIncrement = incr;

		// everything is ok
		setOk(true);
		return SUCCESS;
	}

	int32 PVectorUnsafe::add(void* ptr)
	{
		if (m_uCount >= m_uSize) {
			uint32 new_size = m_uSize + m_uIncrement;

			m_vpPtrs = (void * *) ::realloc(m_vpPtrs,
									new_size * sizeof(void *));
			if (!m_vpPtrs)
				return FAILURE;

			::memset(&m_vpPtrs[m_uSize], 0, m_uIncrement * sizeof(void *));

			m_uSize = new_size;
		}

		m_vpPtrs[m_uCount] = ptr;
		m_uCount += 1;

		// ok
		return (m_uCount - 1);
	}

	int32 PVectorUnsafe::del(uint32 idx)
	{
		if (idx >= m_uCount)
			return FAILURE;

		for (uint32 j = idx; j < m_uCount - 1; j++)
			m_vpPtrs[j] = m_vpPtrs[j + 1];

		m_uCount -= 1;
		return SUCCESS;
	}

	int32 PVectorUnsafe::set(uint32 idx, void* ptr)
	{
		if (idx >= m_uCount)
			return FAILURE;

		m_vpPtrs[idx] = ptr;
		return SUCCESS;
	}

	void* PVectorUnsafe::operator[](uint32 idx)
	{
		return (idx < m_uCount ? m_vpPtrs[idx] : NULL);
	}

	void PVectorUnsafe::clear(void)
	{
		::memset(m_vpPtrs, 0, m_uSize * sizeof(void *));
		m_uCount = 0;
	}

	uint32 PVectorUnsafe::getSize(void)
	{
		return m_uSize;
	}

	uint32 PVectorUnsafe::getCount(void)
	{
		return m_uCount;
	}

	uint32 PVectorUnsafe::getIncrement(void)
	{
		return m_uIncrement;
	}

	void PVectorUnsafe::sort(void)
	{
		_quickSort(0, getCount() - 1, getComparator());
	}

	void PVectorUnsafe::_quickSort(int32 s, int32 d, Comparator* c)
	{
		div_t pos = div(s + d, 2);
		int32 ret = 0;

		int32 i = s;
		int32 j = d;

		void* vX = m_vpPtrs[pos.quot];
		if (!vX)
			return;

		do {
			void* vI = NULL;
			void* vJ = NULL;

			while (1) {
				vI = m_vpPtrs[i];
				if (vI) {
					ret = c->compare(vI, 0, vX, 0);
					if (ret >= 0)
						break;
				}

				i++;
			}

			while (1) {
				vJ = m_vpPtrs[j];
				if (vJ) {
					ret = c->compare(vX, 0, vJ, 0);
					if (ret >= 0)
						break;
				}

				j--;
			}

			if (i <= j) {
				set(i, vJ);
				set(j, vI);

				i++;
				j--;
			}
		} while (i <= j);

		if (s < j)
			_quickSort(s, j, c);
		if (i < d)
			_quickSort(i, d, c);
	}
}
