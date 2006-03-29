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

#include "bufferpool.hpp"
#include "thread.hpp"

namespace uStreamLib {
	BufferPool::BufferPool(void)
		: Object(UOSUTIL_RTTI_BUFFER_POOL), _bufs(NULL), _bcount(0)
	{
		// nothing to do
	}

	BufferPool::~BufferPool(void)
	{
		uint32 bid = 0, i = 0;
		uint32 wait = 10, bc = 0;

		MutexLocker ml(&_mutexReset);

		/*
		 * Here we must free the unlocked buffers first,
		 * then wait some time for further buffers that will
		 * be freed by worker threads.
		 */
		while (bc < _bcount && i++ < wait) {
			DataBuf* buf = (DataBuf*) _psFree.pop();
			if (buf) {
				// get buffer id
				bid = buf->getBID();

				// free selected buffer
				delete _bufs[bid];

				// reset pointer
				_bufs[bid] = NULL;

				// count freed buffers
				bc++;
			} else {
				// wait some time
				Thread::sleep(50);
			}
		}

		/*
		 * Timeout expired, so check which buffers are not
		 * returned home.
		 */
		for (i = 0; i < _bcount; i++) {
			if (_bufs[i]) {
				// DEBUG
				UOSUTIL_DOUT(("BufferPool: Buffer %d is NOT NULL\n", i));
				if (!_bufs[i]->isInUse()) {
					// DEBUG
					UOSUTIL_DOUT(("BufferPool: Buffer %d is NOT IN USE\n", i));
					UOSUTIL_DOUT(("BufferPool: Buffer %d will be DELETED\n", i));
					delete _bufs[i];
				} else {
				        // DEBUG
					UOSUTIL_DOUT(("BufferPool: Buffer %d is IN USE: leaked\n", i));
				}
			} else {
				// DEBUG
				UOSUTIL_DOUT(("BufferPool: Buffer %d is NULL, ignored\n", i));
			}
		}

		/*
		 * Delete other objects.
		 */

		delete[] _bufs;
	}

	int32 BufferPool::init(char* name, uint32 bsize, uint32 bcount,
		uint32 limit)
	{
		int32 ret = 0;

		// do some checks
		if (!bsize)
			bsize = 1;
		if (!bcount)
			bcount = 1;

		// initialize buffers count
		_bcount = bcount;

		// initialize buffer pool's name buffer
		ret = _dbName.init(name);
		if (ret == FAILURE)
			return FAILURE;

		// create buffers array
		_bufs = new DataBuf * [bcount];
		if (!_bufs)
			return FAILURE;

		// initialize buffer ids stack
		ret = _psFree.init(bcount);
		if (ret == FAILURE)
			return FAILURE;

		// initialize semaphores
		ret = _semFree.init(bcount);
		if (ret == FAILURE)
			return FAILURE;

		ret = _semUsed.init(0);
		if (ret == FAILURE)
			return FAILURE;

		// initialize mutex for reset
		ret = _mutexReset.init();
		if (ret == FAILURE)
			return FAILURE;

		// allocate each buffer
		for (uint32 i = 0; i < bcount; i++) {
			// create databuf
			_bufs[i] = new DataBuf();

			// initialize databuf
			ret = _bufs[i]->init(bsize, i, limit, DataBuf::ALLOC_ONUSE, this);
			if (ret == FAILURE)
				return FAILURE;

			// push buffer pointer into the "free buffers" stack
			_psFree.push(_bufs[i]);
		}

		// ok
		setOk(true);
		return SUCCESS;
	}

	int32 BufferPool::resize(uint32 bsize, uint32 bcount)
	{
		// lock mutex for reset
		MutexLocker ml(&_mutexReset);

		// output
		fprintf(stderr, "BufferPool::resize(%u,%u)", bsize, bcount);

		// resize is not implemented
		return FAILURE;
	}

	uint32 BufferPool::getBuffer(void)
	{
		DataBuf* buf = NULL;

		// lock mutex for reset
		MutexLocker ml(&_mutexReset);

		// decrement free buffers counter
		_semFree.wait();

		// get bid from stack
		buf = (DataBuf *) _psFree.pop();

		// check if there are no more free buffers
		if (!buf) {
			fprintf(stderr, "BufferPool: NO MORE FREE BUFFERS...\n");
			return 0;
		}

		// increment used buffers counter
		_semUsed.post();

		// return this bid
		return buf->getBID();
	}

	int32 BufferPool::tryGetBuffer(uint32* bid)
	{
		int32 ret = 0;

		// lock mutex for reset
		MutexLocker ml(&_mutexReset);

		// decrement free buffers counter
		ret = _semFree.tryWait();
		if (ret == FAILURE)
			return FAILURE;

		DataBuf* buf = (DataBuf*) _psFree.pop();
		if (!buf) {
			puts("Critical: null buffer in pool"); return FAILURE;
		}

		*bid = buf->getBID();

		// increment used buffers counter
		_semUsed.post();

		// ok
		return SUCCESS;
	}

	void BufferPool::freeBuffer(uint32 bid)
	{
		int32 ret = 0;

		// lock mutex for reset
		MutexLocker ml(&_mutexReset);

		if (bid < _bcount) {
			// decrement used buffers counter
			_semUsed.wait();

			_bufs[bid]->setCount(0);
			_bufs[bid]->setInUse(false);

			ret = _psFree.push(_bufs[bid]);
			if (ret == FAILURE) {
				fprintf(stderr, "BufferPool: STACK IS FULL.\n");
			}

			// increment free buffers counter
			_semFree.post();
		}
	}

	DataBuf* BufferPool::use(uint32 bid)
	{
		DataBuf* buf = NULL;

		// lock mutex for reset
		MutexLocker ml(&_mutexReset);

		if (bid < _bcount) {
			buf = _bufs[bid];
			buf->setInUse(true);
		}

		// ok
		return buf;
	}

	int32 BufferPool::getBuffersCount(void)
	{
		// lock mutex for reset
		MutexLocker ml(&_mutexReset);
		return _bcount;
	}

	int32 BufferPool::getFreeBuffersCount(void)
	{
		// lock mutex for reset
		MutexLocker ml(&_mutexReset);
		return _psFree.getStackPointer();
	}

	uint32 BufferPool::getBufferSize(uint32 bid)
	{
		uint32 ret = 0;

		// lock mutex for reset
		MutexLocker ml(&_mutexReset);

		if (bid < _bcount)
			ret = _bufs[bid]->getSize();

		// ok
		return ret;
	}

	void BufferPool::reset(void)
	{
		DataBuf* buf = NULL;
		int32 ret = 0;

		// lock mutex for reset
		MutexLocker ml(&_mutexReset);

		// empty buffers stack
		for (uint32 j = 0; j < _bcount; j++) {
			buf = (DataBuf *) _psFree.pop();
			if (!buf)
				break;

			UOSUTIL_DOUT(("Get buffer from the stack: %u\n", buf->getBID()));
		}

		// reset "used" semaphore
		while ((ret = _semUsed.tryWait()) != FAILURE)
			UOSUTIL_DOUT(("Waiting on semaphore \"used\"\n"));

		// reset "free" semaphore    
		while ((ret = _semFree.tryWait()) != FAILURE)
			UOSUTIL_DOUT(("Waiting on semaphore \"free\"\n"));

		// put all buffers in the buffers stack
		for (uint32 i = 0; i < _bcount; i++) {
			// reset buffer
			_bufs[i]->setInUse(false);
			_bufs[i]->setCount(0);

			// push buffer pointer into the "free buffers" stack
			_psFree.push(_bufs[i]);

			// DEBUG
			UOSUTIL_DOUT(("Buffer %u pushed.\n", _bufs[i]->getBID()));
		}

		// reset "free" semaphore    
		for (uint32 k = 0; k < _bcount; k++) {
			UOSUTIL_DOUT(("Posting semaphore \"free\" (%u)\n", k));
			_semFree.post();
		}
	}
}
