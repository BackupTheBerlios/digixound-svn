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

#ifndef BUFFERPOOL_HPP
#define BUFFERPOOL_HPP

#include "databuf.hpp"
#include "pstack.hpp"
#include "semaphore.hpp"
#include "mutex.hpp"

namespace uStreamLib {
	/**
	 * This class manages a pool of buffers which can be
	 * locked or unlocked by different threads. The pool
	 * uses a SharedQueue to store buffer identifiers.
	 * Each buffer is a DataBuf.
	 */
	class US_API_EXPORT BufferPool : public Object {
	public:
		/**
		 * Constructor.
		 */
		BufferPool(void);

		/**
		 * Destroy buffer pool and each buffer.
		 */
		virtual ~BufferPool(void);

		/**
		 * Create a buffer pool.
		 * @param bsize buffer size.
		 * @param bcount buffers count.
		 * @param limit size limit for each buffer.
		 * @return SUCCESS or FAILURE.
		 */
		int32 init(
			char* name,		// buffer pool's name
	  		uint32 bsize,		// single buffer size
	  		uint32 bcount,		// buffers count
	  		uint32 limit = 8388608	// size limit for each buffer
		);

		/**
		 * Resize the whole buffer pool and its buffers.
		 * Don't use this method.
		 * @param bsize new buffer size.
		 * @param bcount new buffers count.
		 * @return SUCCESS or FAILURE.
		 */
		int32 resize(uint32 bsize, uint32 bcount);

		/**
		 * Get the next free buffer.
		 * @return a free buffer id to use with use() method.
		 * May not return failures.
		 */
		uint32 getBuffer(void);

		/**
		 * Get next free buffer.
		 * This method is not blocking.
		 * @return a free buffer id to use with use() method.
		 */
		int32 tryGetBuffer(uint32* bid);

		/**
		 * Free specified buffer.
		 * @param bid the buffer id of the buffer to free.
		 */
		void freeBuffer(uint32 bid);

		/**
		 * Use the specified buffer.
		 * @param bid the buffer id of the buffer to use.
		 * @return a pointer to the DataBuf to use or NULL if bid
		 * is out of range.
		 */
		DataBuf* use(uint32 bid);

		/**
		 * Get buffers count.
		 * @return total buffers in the buffer pool.
		 */
		int32 getBuffersCount(void);

		/**
		 * Get free buffers count.
		 * @return free buffers count.
		 */
		int32 getFreeBuffersCount(void);

		/**
		 * Get actual size of the specified buffer.
		 * @param bid the buffer id of the buffer we need to
		 * know the size.
		 * @return the size of the specified buffer.
		 */
		uint32 getBufferSize(uint32 bid);

		/**
		 * Reset this buffer pool, ie free all buffers.
		 */
		void reset(void);

		/**
		 * Get this buffer pool's name.
		 */
		char* getName(void)
		{
			return _dbName.toString();
		}
	private:
		// no copy constructors
		BufferPool(BufferPool& bp)
			: Object(bp)
		{
		}

		// buffer pool's name
		DataBuf _dbName;

		// buffers array
		DataBuf** _bufs;

		// count of buffers (locked + unlocked)
		uint32 _bcount;

		// free buffer stack
		PStack _psFree;

		// semaphore for counting free buffers
		Semaphore _semFree;

		// semaphore for counting used buffers
		Semaphore _semUsed;

		// mutex to allow thread safe reset
		Mutex _mutexReset;
	};
}

#endif
