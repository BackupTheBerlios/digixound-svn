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
#include <malloc.h>
#include <ctype.h>

#include "vector.hpp"
#include "databuf.hpp"

namespace uStreamLib {
	/*
	 * Vector item structure.
	 */

	struct VectorSlot {
		/* constructor */
		VectorSlot(void)
		{
			bEmpty = true;
		}

		/* container buffer */
		DataBuf dbData;

		/* flag: the slot is empty */
		bool bEmpty;
	};

	/*
	 * Vector implementation.
	 */

	Vector::Vector(void)
		: Object(UOSUTIL_RTTI_VECTOR), m_bSorted(false), m_comp(NULL),
		m_uCount(0)
	{
		// nothing to do
	}

	Vector::~Vector(void)
	{
		for (uint32 j = 0; j < m_pvDB.getCount(); j++) {
			VectorSlot* vs = (VectorSlot*) m_pvDB[j];
			if (vs)
				delete vs;
		}

		m_uCount = 0;
		m_comp = NULL;
	}

	int32 Vector::init(uint32 isize, uint32 cap, uint32 incr)
	{
		int32 ret = 0;

		// initialize main container
		ret = m_pvDB.init(cap, incr);
		if (ret == FAILURE)
			return FAILURE;

		// initialize enumeration object
		ret = m_enValues.init(cap + incr);
		if (ret == FAILURE)
			return FAILURE;

		// initialize work buffer
		ret = m_dbWork.init(4096, 0, 64000000, DataBuf::ALLOC_ONUSE);
		if (ret == FAILURE)
			return FAILURE;

		// preallocate buffers
		for (uint32 j = 0; j < cap; j++) {
			VectorSlot* vs = new VectorSlot();
			if (!vs)
				return FAILURE;

			ret = vs->dbData.init(isize, 0, 65536, DataBuf::ALLOC_ONUSE);
			if (ret == FAILURE) {
				delete vs; return FAILURE;
			}

			ret = m_pvDB.add(vs);
			if (ret == FAILURE) {
				delete vs; return FAILURE;
			}
		}

		// reset member variables
		m_bSorted = false;
		m_comp = NULL;
		m_uCount = 0;

		// ok
		return SUCCESS;
	}

	bool Vector::isSorted(void)
	{
		return m_bSorted;
	}

	int32 Vector::sort(Comparator* comp)
	{
		Comparator* cUse = NULL;

		if (!m_uCount)
			return FAILURE;
		if (!comp && !m_comp)
			return FAILURE;

		if (comp)
			cUse = comp;
		else
			cUse = m_comp;

		_quickSort(0, m_uCount - 1, cUse);

		if (!m_comp)
			m_comp = comp;
		m_bSorted = true;

		return SUCCESS;
	}

	int32 Vector::addElement(char* element, uint32 sz)
	{
		int32 ret = 0;

		VectorSlot* vs = (VectorSlot*) m_pvDB[m_uCount];
		if (!vs) {
			vs = new VectorSlot();
			if (!vs)
				return FAILURE;

			ret = vs->dbData.init(sz, 0, 65536, DataBuf::ALLOC_ONUSE);
			if (ret == FAILURE) {
				delete vs; return FAILURE;
			}

			ret = m_pvDB.add(vs);
			if (ret == FAILURE) {
				delete vs; return FAILURE;
			}
		}

		vs->bEmpty = false;
		ret = vs->dbData.xcopy(element, sz);
		if (ret == FAILURE)
			return FAILURE;

		if (m_bSorted && m_comp)
			sort(m_comp);

		m_uCount += 1;
		return SUCCESS;
	}

	int32 Vector::addElements(char* elements, char* separator, bool trim)
	{
		int32 token_found = 0;
		int32 i = 0, k = 0;

		int32 strsz = (int32) strlen(elements) + 1;
		int32 ret = 0;

		char* in = elements;
		char* token = NULL, * temp = NULL;
		char* first = NULL, * ttemp = NULL;

		char* sep = separator, * sep_cur = sep;
		int32 sepsz = (int32) strlen( separator) + 1, sep_i = 0;

		bool bSorted = m_bSorted;
		m_bSorted = false;

		m_dbWork.setCount(0);

		ret = m_dbWork.realloc(strsz);
		if (ret == FAILURE)
			return FAILURE;

		temp = m_dbWork.toString();
		first = m_dbWork.toString();

		// printf("sepsz = %d, sep = \"%s\"\n",sepsz,sep);

		while (i < strsz && *in) {
			*temp = *in;

			if (*in == '\"') {
				temp = first;

				while (++i < strsz && *(++in) != '\"') {
					*temp = *in; temp++;
				}

				*temp = '\0';
				token_found = 1;
				temp = first;

				in++;
				i++;
			} else {
				while (*in && *sep_cur && *in == *sep_cur) {
					// printf("FOUND %d-th char of %s\n",sep_i,sep);

					sep_cur++;
					sep_i++;

					temp++;
					in++;
					i++;

					*temp = *in;
				}

				if (*in == '\0' || *sep_cur == '\0' || sep_i == sepsz - 1) {
					// printf("FOUND SEPARATOR OR END OF INPUT\n");

					*(temp - sep_i) = '\0';
					temp = first;
					sep_cur = sep;
					sep_i = 0;
					token_found = 1;
				} else {
					temp++;
					in++;
					i++;
				}
			}

			if (*in == '\0') {
				// printf("FOUND END OF INPUT\n");

				if (temp != first)
					*temp = '\0';

				temp = first;
				sep_cur = sep;
				sep_i = 0;
				token_found = 1;
			}

			if (token_found) {
				token = temp;

				k = (int32) strlen(token);
				while (k && isspace(toascii(*token))) {
					token++; k--;
				}

				k = (int32) strlen(token);
				ttemp = &token[k];
				while (k &&
					(isspace(toascii(*ttemp)) || iscntrl(toascii(*ttemp)))) {
					*ttemp = 0; ttemp--;
				}

				// printf("token = ->\"%s\"<-\n",token);

				if (*token) {
					ret = addElement(token);
					if (ret == FAILURE)
						return FAILURE;
				}

				token_found = 0;
			}
		}

		m_bSorted = bSorted;
		if (m_bSorted && m_comp)
			sort(m_comp);

		return SUCCESS;
	}

	int32 Vector::setElementAt(uint32 idx, char* item, uint32 sz)
	{
		if (idx >= m_uCount)
			return FAILURE;

		VectorSlot* vs = (VectorSlot*) m_pvDB[idx];
		if (vs) {
			vs->bEmpty = false;
			return vs->dbData.xcopy(item, sz);
		}

		if (m_bSorted && m_comp)
			sort(m_comp);

		return FAILURE;
	}

	int32 Vector::scan(char* key, uint32 sz, uint32* index, Comparator* c)
	{
		Comparator* cUse = NULL;
		int32 ret = 0;

		if (!c && !m_comp)
			return FAILURE;

		if (!c)
			cUse = m_comp;
		else
			cUse = c;

		for (uint32 j = 0; j < getCount(); j++) {
			VectorSlot* vs = (VectorSlot*) m_pvDB[j];
			if (vs) {
				ret = cUse->compare(vs->dbData.toString(),
								vs->dbData.getCount(), key, sz);
				if (ret == SUCCESS) {
					if (index)
						*index = j;
					return SUCCESS;
				}
			}
		}

		return FAILURE;
	}

	char* Vector::search(char* key, uint32 sz, Comparator* c)
	{
		uint32 index = 0;

		int32 ret = scan(key, sz, &index, c);
		if (ret == FAILURE)
			return NULL;

		return (*this)[index];
	}

	int32 Vector::removeElement(char* element, uint32 sz, Comparator* c)
	{
		uint32 index = 0;

		int32 ret = scan(element, sz, &index, c);
		if (ret == FAILURE)
			return FAILURE;

		return removeElementAt(index);
	}

	int32 Vector::removeElementAt(uint32 idx)
	{
		int32 ret = 0;

		if (idx >= m_uCount)
			return FAILURE;

		VectorSlot* vs = (VectorSlot*) m_pvDB[idx];
		if (vs) {
			delete vs;

			ret = m_pvDB.del(idx);
			if (ret == FAILURE)
				return FAILURE;

			m_uCount -= 1;
			return SUCCESS;
		}

		return FAILURE;
	}

	void Vector::removeAll(void)
	{
		for (uint32 j = 0; j < m_uCount; j++) {
			VectorSlot* vs = (VectorSlot*) m_pvDB[j];
			if (vs) {
				vs->bEmpty = true;
				vs->dbData.setCount(0);
			}
		}

		m_uCount = 0;
	}

	void Vector::removeLast(void)
	{
		removeElementAt(getCount() - 1);
	}

	char* Vector::operator[](uint32 idx)
	{
		if (idx >= m_uCount)
			return NULL;

		VectorSlot* vs = (VectorSlot*) m_pvDB[idx];
		if (vs && !vs->bEmpty)
			return vs->dbData.toString();

		return NULL;
	}

	Enumeration* Vector::elements(void)
	{
		m_enValues.clear();

		for (uint32 i = 0; i < getCount(); i++) {
			VectorSlot* vs = (VectorSlot*) m_pvDB[i];
			if (vs && !vs->bEmpty) {
				m_enValues.addElement(vs->dbData.toString(),
							vs->dbData.getCount());
			}
		}

		m_enValues.rewind();
		return &m_enValues;
	}

	void Vector::_quickSort(int32 s, int32 d, Comparator* c)
	{
		div_t pos = div(s + d, 2);
		int32 ret = 0;

		int32 i = s;
		int32 j = d;

		VectorSlot* vsX = (VectorSlot*) m_pvDB[pos.quot];
		if (!vsX || (vsX && vsX->bEmpty))
			return;

		char* strX = vsX->dbData.toString();
		uint32 x_sz = vsX->dbData.getCount();

		do {
			VectorSlot* vsI = NULL;
			VectorSlot* vsJ = NULL;

			char* strI = NULL;
			uint32 i_sz = 0;

			char* strJ = NULL;
			uint32 j_sz = 0;

			while (1) {
				vsI = (VectorSlot *) m_pvDB[i];
				if (vsI && !vsI->bEmpty) {
					strI = vsI->dbData.toString();
					i_sz = vsI->dbData.getCount();

					ret = c->compare(strI, i_sz, strX, x_sz);
					if (ret >= 0)
						break;
				}

				i++;
			}

			while (1) {
				vsJ = (VectorSlot *) m_pvDB[j];
				if (vsJ && !vsJ->bEmpty) {
					strJ = vsJ->dbData.toString();
					j_sz = vsJ->dbData.getCount();

					ret = c->compare(strX, x_sz, strJ, j_sz);
					if (ret >= 0)
						break;
				}

				j--;
			}

			if (i <= j) {
				m_pvDB.set(i, vsJ);
				m_pvDB.set(j, vsI);

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
