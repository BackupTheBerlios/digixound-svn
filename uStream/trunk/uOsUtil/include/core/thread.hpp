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

#ifndef THREAD_HPP
#define THREAD_HPP

#include "semaphore.hpp"
#include "databuf.hpp"

namespace uStreamLib {
	/*
	 * Forward declaration for implementation.
	 */
	class Impl_Thread;

	/**
	 * This class provides a simple thread abstraction.
	 */
	class US_API_EXPORT Thread : public Object {
	public:
		/**
		 * Constructor.
		 */
		Thread(void);

		/**
		 * Destructor.
		 */
		virtual ~Thread(void);

		/**
		 * Create a thread with specified name.
		 * @param name thread's name.
		 */
		int32 init(char* name);

		/**
		 * Wait for some time.
		 * @param ms time to wait for in milliseconds.
		 */
		static void sleep(int32 ms);

		/**
		 * Get thread's name.
		 */
		char* getName(void)
		{
			return _dbName.toString();
		}

		/**
		 * Set thread's name.
		 */
		void setName(char* name)
		{
			_dbName.copy(name);
		}

		/**
		 * Start this thread.
		 */
		void start(void)
		{
			_semStart.post();
		}

		/**
		 * Make this thread terminate.
		 */
		void doTerminate(void)
		{
			_semTerminate.post();
		}

		/**
		 * Don't execute run.
		 */
		void bypassRun(void)
		{
			_bBypassRun = true;
		}

		/**
		 * Check if this thread will execute run after started.
		 */
		bool getBypassRun(void)
		{
			return _bBypassRun;
		}

		/**
		 * Detach this thread.
		 */
		void detach(void);

		/**
		 * Join caller thread to this thread.
		 */
		void join(void** ret_data);

		/**
		 * Cancel thread.
		 */
		void cancel(void);

		/**
		 * Override this method with your own code.
		 * This is the thread entry point.
		 */
		virtual void run(void);

		/**
		 * Reserved public method.
		 */
		void waitForStart(void)
		{
			_semStart.wait();
		}

		/**
		 * Reserved public method.
		 */
		void threadProc(void);
		
	private:
		/* copy constructor not available */
		Thread(Thread&)
			: Object(UOSUTIL_RTTI_THREAD)
		{
		}

		/* specific implementation */
		Impl_Thread* _impl;

		// thread name
		DataBuf _dbName;

		// start semaphore
		Semaphore _semStart;

		// termination semaphore
		Semaphore _semTerminate;

		// flag: bypass run method
		bool _bBypassRun; 

		// flag: this thread is a dummy SELF thread
		bool _bIsSelf;

		// current thread used by getCurrent()
		static Thread* _current;
	};
}

#endif
