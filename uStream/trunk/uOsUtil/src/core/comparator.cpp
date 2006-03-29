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

#include "comparator.hpp"

namespace uStreamLib {
	Comparator::Comparator(void)
		: Object(UOSUTIL_RTTI_COMPARATOR)
	{
		// nothing to do
	}

	Comparator::~Comparator(void)
	{
		// nothing to do
	}

	int32 MemoryComparator::compare(void* a, uint32 a_sz, void* b, uint32 b_sz)
	{
		if (a_sz > b_sz)
			return  1;
		if (a_sz < b_sz)
			return -1;

		return ::memcmp(a, b, a_sz);
	}

	int32 StringComparator::compare(void* a, uint32 a_sz, void* b, uint32 b_sz)
	{
		char* strA = (char*) a;
		char* strB = (char*) b;

		return ::strcmp(strA, strB);
	}

	int32 IntegerComparator::compare(void* a, uint32 a_sz, void* b,
		uint32 b_sz)
	{
		int32 iA = 0;
		int32 iB = 0;

		memcpy(&iA, a, sizeof(iA));
		memcpy(&iB, b, sizeof(iB));

		if (iA > iB)
			return  1;
		if (iA < iB)
			return -1;

		return 0;
	}
}
