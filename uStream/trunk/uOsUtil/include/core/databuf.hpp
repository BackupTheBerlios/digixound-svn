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

#ifndef DATABUF_HPP
#define DATABUF_HPP

#include <stdio.h>
#include <string.h>

#include "object.hpp"

namespace uStreamLib {
	/*
	 * Forward declaration of BufferPool.
	 */
	class BufferPool;

	/**
	 * This class can create, destroy and use memory buffers
	 * in a controlled way. Each buffer has a size and a buffer id.
	 * A buffer can be larger than the valid bytes it contains: this
	 * is expressed by two properties: count and size. The count
	 * property is the number of valid bytes (<= size) that the buffer
	 * contains.
	 *
	 * This class is not thread safe.
	 */
	class US_API_EXPORT DataBuf : public Object {
	public:
		/*
		 * Friend classes.
		 */
		friend class BufferPool;

		/**
		 * Constants that specify the allocation method. Allocation
		 * methods are useful when DataBuf is used with BufferPool
		 * class. Memory is allocated on demand or preallocated in
		 * the constructor.
		 */
		enum AllocStrategy { 
		/**
		 * Specify this allocation method if you want the DataBuf
		 * to allocate memory in the constructor.
		 */
		ALLOC_ONCREATE = 1,
		/**
		 * Specify this allocation method if you need that the DataBuf
		 * allocates memory when copy(), xcopy() and merge() methods
		 * are invoked.
		 */
		ALLOC_ONUSE = 2 };

		/**
		 * Constructor.
		 */
		DataBuf(void);

		/**
		 * Destructor.
		 */
		virtual ~DataBuf(void);

		/**
		 * Create a buffer with the specified size, buffer id and limit.
		 * @param size the initial size of the buffer.
		 * @param bid the buffer id: this is useful for identifing the buffer
		 * during debugging and is heavily used by the BufferPool class to
		 * identify a buffer in the pool.
		 * @param limit a buffer can be limited in size: it cannot grow to
		 * more than limit size. Pay attention to this parameter: if you don't
		 * know how your buffer is going to grow at runtime, set this to an higher
		 * value than default 64k.
		 * @return SUCCESS or FAILURE.
		 */
		int32 init(uint32 size, uint32 bid = 0, uint32 limit = 65536,
			AllocStrategy strategy = ALLOC_ONCREATE, BufferPool* bp = NULL);

		/**
		 * Create a buffer that will contain the specified null terminated string.
		 * @param string a null terminated string. strlen(string) + 1 bytes will
		 * be allocated.
		 * @return SUCCESS or FAILURE.
		 */
		int32 init(char* string);

		/**
		 * Create a buffer from another one.
		 * @return SUCCESS or FAILURE.
		 */
		int32 init(DataBuf& db);

		/**
		 * Get the parent buffer pool, if any.
		 * @return a pointer to a buffer pool or NULL.
		 */
		BufferPool* getBufferPool(void)
		{
			return m_pbpParent;
		}

		/**
		 * Check if databuf is in use.
		 */
		bool isInUse(void)
		{
			return m_bInUse;
		}

		/**
		 * Signal that this buffer is in use.
		 */
		void setInUse(bool value)
		{
			m_bInUse = value;
		}

		/**
		 * Get the buffer id.
		 */
		uint32 getBID(void)
		{
			return m_uBID;
		}

		/**
		 * Set the buffer id.
		 */
		void setBID(uint32 id)
		{
			m_uBID = id;
		}

		/**
		 * Get buffer size.
		 */
		uint32 getSize(void)
		{
			return m_uSize;
		}

		/**
		 * Set the limit (in bytes).
		 */
		void setLimit(uint32 value)
		{
			m_uLimit = value;
		}

		/**
		 * Get the limit.
		 */
		uint32 getLimit(void)
		{
			return m_uLimit;
		}

		/**
		 * Get the internal char* pointer. Notice that after a realloc()
		 * the returned pointer cannot be used any more and you must
		 * recall the getAddr() method.
		 * @return a valid pointer to the data contained in the buffer or
		 * NULL if ALLOC_ONUSE is specified and none of copy(), xcopy() and
		 * merge() methods have been called.
		 */
		void* getAddr(void)
		{
			return (void *) m_strBlock;
		}

		/**
		 * Get a pointer to the element at pos.
		 */
		char* operator[](uint32 pos)
		{
			if (!m_strBlock)
				return NULL;
			if (pos >= m_uSize)
				return NULL;

			return &m_strBlock[pos];
		}

		/**
		 * Get the number of "valid bytes" in the buffer. A buffer can be
		 * larger than its content size.
		 * @return number of valid bytes.
		 */
		uint32 getCount(void)
		{
			return m_uCount;
		}

		/**
		 * Set the number of valid bytes. This method is used when you fill
		 * the buffer by other means than those provided by this class.
		 * When filling the buffer this way, you must set the right valid
		 * bytes count by invoking setCount().
		 * @param count number of valid bytes.
		 */
		void setCount(uint32 count)
		{
			if (count <= m_uSize)
				m_uCount = count;
			else
				m_uCount = m_uSize;
		}

		/**
		 * Copy size bytes in the buffer. The bytes are read from the data
		 * array. This method does not stretch the buffer, use xcopy() instead.
		 * @param data a pointer to a buffer containing the bytes to be copied.
		 * @param size number of bytes to copy.
		 */
		void copy(char* data, uint32 size);

		/**
		 * Copy a null terminated string into this buffer.
		 * @param string a null terminated string.
		 */
		void copy(char* string)
		{
			copy(string, (uint32) strlen(string) + 1);
		}

		/**
		 * Copy a databuf into this one.
		 * @param b a pointer to the DataBuf containing the
		 * data to be copied.
		 */
		void copy(DataBuf* b)
		{
			copy((char *) b->getAddr(), b->getCount());
		}

		/**
		 * This method is like the copy() one, but stretches the
		 * buffer if source data are more than this buffer's size.
		 * @return SUCCESS or FAILURE if no memory is available
		 * to stretch the buffer.
		 */
		int32 xcopy(char* data, uint32 size);

		/**
		 * This method is like the copy() counterpart, but sthretches
		 * the buffer if source data are more than this buffer's size.
		 * @return SUCCESS or FAILURE if no memory is available
		 * to stretch the buffer.
		 */
		int32 xcopy(char* string)
		{
			return xcopy(string, (uint32) strlen(string) + 1);
		}

		/**
		 * This method is like the copy() counterpart. Bytes are read
		 * from a source databuf.
		 * @return SUCCESS or FAILURE if no memory is available
		 * to stretch the buffer.
		 */
		int32 xcopy(DataBuf* b)
		{
			return xcopy((char *) b->getAddr(), b->getCount());
		}

		/**
		 * Append bytes to the buffer. The buffer is stretched if necessary.
		 * @return SUCCESS or FAILURE if no memory is available
		 * to stretch the buffer.
		 */
		int32 merge(char* data, uint32 size);

		/**
		 * Append a null terminated string to the buffer. The buffer is stretched
		 * if necessary.
		 * @return SUCCESS or FAILURE if no memory is available
		 * to stretch the buffer.
		 */
		int32 merge(char* string)
		{
			return merge(string, (uint32) strlen(string));
		}

		/**
		 * Append bytes to the buffer. The buffer is stretched if necessary.
		 * Bytes are read from a source databuf.
		 * @return SUCCESS or FAILURE if no memory is available
		 * to stretch the buffer.
		 */
		int32 merge(DataBuf* b)
		{
			return merge((char *) b->getAddr(), b->getCount());
		}

		/**
		 * Move size byte from "from" to "to".
		 * This method moves bytes in the buffer. It does not check
		 * bounds so it can sigsegv if you pass wrong parameters.
		 * @param to the destination index in the buffer.
		 * @param from the source index in the buffer.
		 * @param size the number of bytes to move.
		 */
		void move(int32 to, int32 from, uint32 size);

		/**
		 * Move size byte from "from" to the beginning of the buffer.
		 * This method moves bytes in the buffer. It does not check
		 * bounds so it can sigsegv if you pass wrong parameters.
		 * @param from the source index in the buffer.
		 * @param size the number of bytes to move.
		 */
		void moveOnStart(int32 from, uint32 size);

		/**
		 * Fill the buffer with the c character.
		 * If ALLOC_ONUSE is specified, this method does NOT allocate memory
		 * and does nothing.
		 */
		void set(int32 c);

		/**
		 * Fill the buffer with the c character starting at offset offset.
		 * If ALLOC_ONUSE is specified, this method does NOT allocate memory
		 * and does nothing.
		 */
		void set(uint32 offset, int32 c);

		/**
		 * Realloc the buffer. If size is equal to this buffer's size,
		 * no reallocation is done.
		 * @return SUCCESS or FAILURE if no memory is available
		 * to stretch the buffer.
		 */
		int32 realloc(uint32 size);

		/**
		 * Print on stdout each byte in the buffer using hex notation.
		 */
		void printHex(void);

		/**
		 * Utility method to get the string representation of the buffer.
		 * Use this method when you know this buffer is a null terminated
		 * string.
		 * @return a valid pointer to the data contained in the buffer or
		 * NULL if ALLOC_ONUSE is specified and none of copy(), xcopy() and
		 * merge() methods have been called.
		 */
		char* toString(void)
		{
			return m_strBlock;
		}
	private:
		/* no copy constructor available */
		DataBuf(DataBuf& db)
			: Object(db)
		{
		}

		/* pointer to the parent buffer pool */
		BufferPool* m_pbpParent;

		/* numeric identifier */
		uint32 m_uBID;

		/* ptr to a memory chunk */
		char* m_strBlock;

		/* size of the buffer */
		uint32 m_uSize;

		/* valid items count in the buffer */
		uint32 m_uCount;

		/* buffer size limit */
		uint32 m_uLimit;

		/* flag: in use */
		bool m_bInUse;

		/* memory allocation strategy */
		AllocStrategy m_asStrategy;
	};
}

#endif
