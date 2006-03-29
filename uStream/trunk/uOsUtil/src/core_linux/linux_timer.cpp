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

#include "linux_timer.hpp"

namespace uStreamLib {
	Impl_Timer::Impl_Timer(void)
		: _u_telapsed(0)
	{
		// nothing to do
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
		gettimeofday(&_tv1, NULL);
	}

	uint32 Impl_Timer::getElapsed(void)
	{
		return _u_telapsed;
	}

	void Impl_Timer::stop(TimeDesc* td)
	{
		struct timeval res;
		div_t val;

		gettimeofday(&_tv2, NULL);

		res.tv_sec = _tv2.tv_sec - _tv1.tv_sec;
		res.tv_usec = _tv2.tv_usec - _tv1.tv_usec;

		if (res.tv_usec < 0) {
			res.tv_sec -= 1;
			res.tv_usec += 1000000;
		}

		_u_telapsed = (_tv2.tv_sec * 1000000 + _tv2.tv_usec) -
			(_tv1.tv_sec * 1000000 + _tv1.tv_usec);

		val = div(res.tv_sec, 3600);
		td->hours = val.quot;
		td->sec = val.rem;

		val = div(td->sec, 60);
		td->min = val.quot;
		td->sec = val.rem;

		val = div(res.tv_usec, 1000);
		td->msec = val.quot;
		td->usec = val.rem;
	}
}
