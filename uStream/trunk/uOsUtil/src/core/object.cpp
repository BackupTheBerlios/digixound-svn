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

#include "object.hpp"

namespace uStreamLib {
	Object::Object(uint32 class_id)
		: _uClassID(class_id), _uErrorCode(0), _bIsOk(false)
	{
		// nothing to do
	}

	Object::Object(Object& obj)
		: _uClassID(obj._uClassID), _uErrorCode(obj._uErrorCode),
		_bIsOk(obj._bIsOk)
	{
		// nothing to do
	}

	Object::~Object(void)
	{
		// nothing to do
	}
}
