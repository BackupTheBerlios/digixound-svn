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

#ifndef SHAREDTIMER_HPP
#define SHAREDTIMER_HPP

#include "timer.hpp"
#include "mutex.hpp"

namespace uStreamLib {
	/**
	 * Shared timer. This timer can be shared among threads.
	 */
	class US_API_EXPORT SharedTimer : public Timer, public Mutex {
	public:
		/**
		 * Constructor.
		 */
		SharedTimer(void);

		/**
		 * Destructor.
		 */
		virtual ~SharedTimer(void);

		/**
		 * Initialize shared timer.
		 */
		int32 init(void);

		/**
		 * Reset timer.
		 */
		void reset(void)
		{
			MutexLocker ml(this); Timer::reset();
		}

		/**
		 * Start timer.
		 */
		void start(void)
		{
			MutexLocker ml(this); Timer::start();
		}

		/**
		 * Get elapsed time.
		 */
		uint32 getElapsed(void)
		{
			MutexLocker ml(this); return Timer::getElapsed();
		}

		/**
		 * Stop timer.
		 */
		void stop(TimeDesc* td)
		{
			MutexLocker ml(this); Timer::stop(td);
		}
	};
}

#endif
