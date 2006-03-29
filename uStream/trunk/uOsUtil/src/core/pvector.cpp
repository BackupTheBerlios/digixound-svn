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

#include "pvector.hpp"

namespace uStreamLib {
	PVector::PVector(void)
	{
		PVectorUnsafe::setClassID(UOSUTIL_RTTI_PVECTOR);
	}

	PVector::~PVector(void)
	{
		// lock mutex
		MutexLocker ml(this);
	}

	int32 PVector::init(uint32 init_size, uint32 incr)
	{
		int32 ret = 0;

		// initialize parent
		ret = PVectorUnsafe::init(init_size, incr);
		if (ret == FAILURE)
			return FAILURE;

		// initialize parent
		ret = Mutex::init();
		if (ret == FAILURE)
			return FAILURE;

		// everything is ok
		return SUCCESS;
	}
}