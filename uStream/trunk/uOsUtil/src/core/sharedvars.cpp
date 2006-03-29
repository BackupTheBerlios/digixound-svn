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

#include "sharedvars.hpp"

namespace uStreamLib {
	SharedInt::SharedInt(void)
	{
		setClassID(UOSUTIL_RTTI_SHARED_INT);
	}

	SharedInt::~SharedInt(void)
	{
		// nothing to do
	}

	int32 SharedInt::init(int32 value)
	{
		_value = value;
		return Mutex::init();
	}

	SharedUint::SharedUint(void)
	{
		setClassID(UOSUTIL_RTTI_SHARED_UINT);
	}

	SharedUint::~SharedUint(void)
	{
		// nothing to do
	}

	int32 SharedUint::init(uint32 value)
	{
		_value = value;
		return Mutex::init();
	}

	SharedDouble::SharedDouble(void)
	{
		setClassID(UOSUTIL_RTTI_SHARED_DOUBLE);
	}

	SharedDouble::~SharedDouble(void)
	{
		// nothing to do
	}

	int32 SharedDouble::init(double value)
	{
		_value = value;
		return Mutex::init();
	}

	SharedString::SharedString(void)
	{
		setClassID(UOSUTIL_RTTI_SHARED_STRING);
	}

	SharedString::~SharedString(void)
	{
		// nothing to do
	}

	int32 SharedString::init(char* value)
	{
		int32 ret = 0;

		// initialize parent
		ret = Mutex::init();
		if (ret == FAILURE)
			return FAILURE;

		// initialize buffers
		if (value)
			return _value.init(value);

		// initialize with size
		ret = _value.init(30);
		if (ret == FAILURE)
			return FAILURE;

		// zeroize buffer
		_value.set(0);

		// ok
		return SUCCESS;
	}

	int32 SharedString::init(uint32 size)
	{
		int32 ret = 0;

		// initialize parent
		ret = Mutex::init();
		if (ret == FAILURE)
			return FAILURE;

		// initialize buffer
		ret = _value.init(size);
		if (ret == FAILURE)
			return FAILURE;

		// zeroize buffer
		_value.set(0);

		// ok
		return SUCCESS;
	}
}
