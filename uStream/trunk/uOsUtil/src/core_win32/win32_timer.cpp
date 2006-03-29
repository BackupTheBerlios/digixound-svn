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

#include "win32_timer.hpp"

namespace uStreamLib {
	Impl_Timer::Impl_Timer(void)
		: _u_telapsed(0)
	{
		QueryPerformanceFrequency(&_pfreq);
	}

	Impl_Timer::~Impl_Timer(void)
	{
		// nothing to do
	}

	void Impl_Timer::reset(void)
	{
		_u_telapsed = 0;
	}

	void Impl_Timer::start(void)
	{
		QueryPerformanceCounter(&_tstart);
	}

	uint32 Impl_Timer::getElapsed(void)
	{
		return _u_telapsed;
	}

	void Impl_Timer::stop(TimeDesc* td)
	{
		DWORD delta = 0, msec = 0;
		double seconds = 0.0;
		div_t val;

		memset(td, 0, sizeof(TimeDesc));

		QueryPerformanceCounter(&_tstop);

		delta = _tstop.LowPart - _tstart.LowPart;
		seconds = (double) ((double) delta / (double) _pfreq.LowPart);
		msec = (DWORD) ((double) seconds * 1.0e3);
		_u_telapsed = (uint32) (seconds * 1.0e6);

		/*
			printf("delta       = %ld\n", delta);
			printf("seconds     = %f\n",  seconds);
			printf("msec        = %ld\n", msec);
			printf("_u_telapsed = %ld\n", _u_telapsed);
		 */

		val = div(msec, 3600000);
		td->hours = val.quot;
		td->msec = val.rem;

		val = div(td->msec, 60000);
		td->min = val.quot;
		td->msec = val.rem;

		val = div(td->msec, 1000);
		td->sec = val.quot;
		td->msec = val.rem;
		td->usec = 0;

		/*
			printf("hours       = %d\n", td->hours);
			printf("minutes     = %d\n", td->min);
			printf("seconds     = %d\n", td->sec);
			printf("msec        = %d\n", td->msec);
		 */
	}
}
