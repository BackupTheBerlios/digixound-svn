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

#ifndef TIMER_HPP
#define TIMER_HPP

#include "object.hpp"
#include "timedesc.hpp"

/*
 * Here, we choose the right implementation using
 * conditional compilation.
 */

#if defined(_WIN32) || defined(WIN32)
#include "win32_timer.hpp"
#else
#include "linux_timer.hpp"
#endif

/*
 * This class is a proxy for specific implementation.
 */

namespace uStreamLib {
	/**
	 * Timer object for short time interval measurements.
	 */
	class US_API_EXPORT Timer : public Object {
	public:
		/**
		 * Constructor.
		 */
		Timer(void);

		/**
		 * Destructor.
		 */
		virtual ~Timer(void);

		/**
		 * Reset timer.
		 */
		void reset(void)
		{
			_impl->reset();
		}

		/**
		 * Start timer.
		 */
		void start(void)
		{
			_impl->start();
		}

		/**
		 * Stop timer and populate time description structure.
		 * @param td a pointer to a TimeDesc structure which contains
		 * time information in hours, minutes, seconds, milliseconds and
		 * microseconds. Latter may be not available on all platforms.
		 */
		void stop(TimeDesc* td)
		{
			_impl->stop(td);
		}

		/**
		 * Get elapsed time.
		 * For this method to work, you need to invoke stop() before
		 * invoking getElapsed().
		 * @return elapsed time in microseconds.
		 */
		uint32 getElapsed(void)
		{
			return _impl->getElapsed();
		}
		
	private:
		/* pointer to specific implementation */
		Impl_Timer* _impl;
	};
}

#endif
