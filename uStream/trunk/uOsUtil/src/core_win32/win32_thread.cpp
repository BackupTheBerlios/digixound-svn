/*
  uSTREAM LIGHT-WEIGHT STREAMING ARCHITECTURE
  Copyright (C) 2005 Luis Serrano (luis@kontrol-dj.com) 
  
  Based on DANUBIO STREAMING ARCHITECTURE by Michele Iacobellis (m.iacobellis@nexotech.it)
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
*/

#include "win32_thread.hpp"
#include "thread.hpp"

namespace uStreamLib {
	/*
	 * Thread procedure.
	 */

	DWORD WINAPI thread_proc(LPVOID thread_object)
	{
		Thread* t = (Thread*) thread_object;
		if (t)
			t->threadProc();

		return 0;
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
		_tid = ::CreateThread(NULL, 0, thread_proc, // thread entry point
		t,  		 // thread data (t object)
		0, &_id);
		if (_tid == NULL)
			return FAILURE;

		return SUCCESS;
	}

	void Impl_Thread::sleep(int32 milliseconds)
	{
		::Sleep(milliseconds);
	}

	void Impl_Thread::detach(void)
	{
		// not implemented
	}

	void Impl_Thread::join(void**)
	{
		HANDLE handles[1];

		handles[0] = _tid;
		MsgWaitForMultipleObjects(1, handles, TRUE, INFINITE, QS_ALLINPUT);
	}

	void Impl_Thread::cancel(void)
	{
		TerminateThread(_tid, 0);
	}
}
