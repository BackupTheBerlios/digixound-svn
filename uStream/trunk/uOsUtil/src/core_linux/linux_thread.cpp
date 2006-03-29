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

#include <pthread.h>

#include "thread.hpp"
#include "linux_thread.hpp"

namespace uStreamLib {
	/*
	 * Thread procedure.
	 */

	static void* thread_proc(void* thread_object)
	{
		Thread* t = (Thread*) thread_object;
		if (t)
			t->threadProc();

		return NULL;
	}

	Impl_Thread::Impl_Thread(void)
	{
		// nothing to do
	}

	Impl_Thread::~Impl_Thread(void)
	{
		// nothing to do
	}

	int32 Impl_Thread::init(Thread* t)
	{
		int32 ret;

		// create posix thread
		ret = pthread_create(&_tid, NULL, thread_proc, t);
		if (ret)
			return FAILURE;

		// configure thread
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

		// ok
		return SUCCESS;
	}

	void Impl_Thread::sleep(int32 milliseconds)
	{
		struct timeval to;

		to.tv_sec = 0;
		to.tv_usec = milliseconds * 1000;

		select(1, NULL, NULL, NULL, &to);
	}

	void Impl_Thread::detach(void)
	{
		::pthread_detach(_tid);
	}

	void Impl_Thread::join(void** ret_data)
	{
		::pthread_join(_tid, ret_data);
	}

	void Impl_Thread::cancel(void)
	{
		::pthread_cancel(_tid);
	}
}
