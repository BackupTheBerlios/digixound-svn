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

#ifndef IMPL_SEMAPHORE_HPP
#define IMPL_SEMAPHORE_HPP

#include "object.hpp"

namespace uStreamLib {
	class US_API_EXPORT Impl_Semaphore {
	public:
		/* constructor */
		Impl_Semaphore(void);

		/* destructor */
		~Impl_Semaphore(void);

		/* initialization */
		int32 init(uint32 init_value, bool process_shared);

		/* public interface */
		int32 wait(void);
		int32 tryWait(void);
		int32 post(void);
		uint32 getValue(void);

		/* get error string */
		char* getErrorString();
	private:
		// win32 semaphore descriptor
		HANDLE _sem;

		// value in the semaphore
		int32 _value;
	};
}

#endif
