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

#ifndef IMPL_MUTEX_HPP
#define IMPL_MUTEX_HPP

#include <pthread.h>

#include "typedefs.hpp"

namespace uStreamLib {
	class Impl_Mutex {
	public:
		/* constructor */
		Impl_Mutex(void);

		/* destructor */
		~Impl_Mutex(void);

		/* initialization */
		int32 init(void);

		/* public interface */
		int32 lock(void);
		int32 tryLock(void);
		int32 unlock(void);

		/* get error string */
		char* getErrorString(void);
	private:
		/* mutex descriptor (posix threads) */
		pthread_mutex_t _mutex;
	};
}

#endif
