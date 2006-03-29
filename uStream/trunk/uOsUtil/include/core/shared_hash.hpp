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

#ifndef SHAREDHASH_HPP
#define SHAREDHASH_HPP

#include "hash.hpp"
#include "thread.hpp"
#include "mutex.hpp"

namespace uStreamLib {
	class US_API_EXPORT SharedHash : public Hash {
	public:
		/**
		 * Enumeration of hash status.
		 */
		enum { 
			/**
			 * The hash table is busy, ie locked by another
			 * thread.
			 */
			HASH_BUSY = 100 };

		/**
		 * Constructor.
		 */
		SharedHash(void);

		/**
		 * Destructor.
		 */
		virtual ~SharedHash(void);

		/**
		 * Initialize hash table.
		 * @param hash_size hash buckets count.
		 */
		int32 init(uint32 hash_size);

		/**
		 * Lock the table.
		 */
		int32 lock(void)
		{
			return _mutex.lock();
		}

		/**
		 * Unlock the table.
		 */
		int32 unlock(void)
		{
			return _mutex.unlock();
		}

		/**
		 * Check if table can be locked and lock it.
		 * Return FAILURE if the table cannot be locked.
		 */
		int32 tryLock(void)
		{
			return _mutex.tryLock();
		}

		/**
		 * Get the value for the specified key.
		 */
		char* get(
			char* key, // key to get
			uint32 key_size, // key size in bytes
			bool try_lock = false  // whether to lock or try a lock
		);

		/**
		 * Get the value for the specified key.
		 * @param key is treated as a null terminated string.
		 */
		char* get(char* key, bool try_lock = false)
		{
			return SharedHash::get(key, (uint32) strlen(key) + 1, try_lock);
		}

		/**
		 * Get the value and its size for the specified key.
		 */
		char* get(
			char* key, // key to get
			uint32 key_size, // key size in bytes
			uint32* val_size, // value size in bytes
			bool try_lock = false  // whether to lock or try a lock
		);

		/**
		 * Get the value and its size for the specified key.
		 */
		char* get(char* key, uint32* val_size, bool try_lock)
		{
			return SharedHash::get(key, (uint32) strlen(key) + 1, val_size,
					try_lock);
		}

		/**
		 * Put the specified value under the specified key.
		 */
		int32 put(
			char* key, // key to put
			uint32 key_size, // key size in bytes
			char* val, // value to put (cast to char* may be needed)
			uint32 val_size, // value size in bytes
			bool replace = false, // whether to replace an existing key
			bool try_lock = false   // whether to lock or try a lock
		);

		/**
		 * Put the specified value under the specified key.
		 * @param key is treated as a null terminated string.
		 */
		int32 put(char* key, char* val, uint32 val_size, bool replace = false,
			bool try_lock = false)
		{
			return SharedHash::put(key, (uint32) strlen(key) + 1, val,
					val_size, replace, try_lock);
		}

		/**
		 * Put the specified value under the specified key. The value is a pointer
		 * so in the table is stored ONLY THE POINTER, NOT THE VALUE.
		 */
		int32 pput(char* key, uint32 key_size, char* val,
			bool replace = false, bool try_lock = false);

		/**
		 * Put the specified value under the specified key. The value is a pointer
		 * so in the table is stored ONLY THE POINTER, NOT THE VALUE.
		 * @param key is treated as a null terminated string.
		 */
		int32 pput(char* key, char* val, bool replace = false,
			bool try_lock = false)
		{
			return SharedHash::pput(key, (uint32) strlen(key) + 1, val,
					replace, try_lock);
		}

		/**
		 * Remove value and key for the specified key.
		 */
		int32 del(
			char* key, // key to remove
			uint32 key_size, // key size in bytes
			bool try_lock = false  // whether to lock or try a lock
		);

		/**
		 * Remove value and key for the specified key.
		 * @param key is treated as a null terminated string.
		 */
		int32 del(char* key, bool try_lock = false)
		{
			return SharedHash::del(key, (uint32) strlen(key) + 1, try_lock);
		}

		/**
		 * Clear the table.
		 */
		void clear(void)
		{
			MutexLocker ml(&_mutex); Hash::clear();
		}
		
	private:
		// copy constructor not available
		SharedHash(SharedHash&)
		{
		}

		// table protection mutex
		Mutex _mutex;
	};
}

#endif
