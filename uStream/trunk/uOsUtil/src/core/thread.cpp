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

#include "thread.hpp"

/*
 * Here, we choose the right implementation using
 * conditional compilation.
 */

#if defined(_WIN32) || defined(WIN32)
#include "win32_thread.hpp"
#else
#include "linux_thread.hpp"
#endif

/*
 * Implementation.
 */

namespace uStreamLib {
	Thread::Thread(void)
		: Object(UOSUTIL_RTTI_THREAD), _bBypassRun(false), _bIsSelf(false)
	{
		_impl = new Impl_Thread();
	}

	Thread::~Thread(void)
	{
		if (!_bIsSelf)
			_semTerminate.wait();
		delete _impl;
	}

	int32 Thread::init(char* name)
	{
		int32 ret;

		// initialize name buffer
		ret = _dbName.init(name);
		if (ret == FAILURE)
			return FAILURE;

		// initialize start semaphore
		ret = _semStart.init(0);
		if (ret == FAILURE)
			return FAILURE;

		// initialize termination semaphore
		ret = _semTerminate.init(0);
		if (ret == FAILURE)
			return FAILURE;

		// initialize members
		_bBypassRun = false;
		_current = NULL;
		_bIsSelf = false;

		// invoke implementation
		return _impl->init(this);
	}

	void Thread::sleep(int32 ms)
	{
		Impl_Thread::sleep(ms);
	}

	void Thread::detach(void)
	{
		_impl->detach();
	}

	void Thread::join(void** ret_data)
	{
		_impl->join(ret_data);
	}

	void Thread::cancel(void)
	{
		_impl->cancel();
	}

	void Thread::run(void)
	{
		// nothing to do
	}

	void Thread::threadProc(void)
	{
		// DEBUG
		UOSUTIL_DOUT(("%s: Waiting for start...\n", getName()));

		// wait for start signal
		waitForStart();

		// DEBUG
		UOSUTIL_DOUT(("%s: Executing run ? %d\n", getName(), getBypassRun()));

		// check if run must be bypassed
		if (!getBypassRun())
			run();

		// DEBUG
		UOSUTIL_DOUT(("%s: Signal termination\n"));

		// ok, terminate
		doTerminate();

		// DEBUG
		UOSUTIL_DOUT(("%s: exiting\n", getName()));
	}
	
	Thread *Thread::_current = NULL;
}
