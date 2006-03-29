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

#include "shared_hash.hpp"

namespace uStreamLib {
	SharedHash::SharedHash(void)
	{
		setClassID(UOSUTIL_RTTI_SHARED_HASH);
	}

	SharedHash::~SharedHash(void)
	{
		// nothing to do
	}

	int32 SharedHash::init(uint32 hash_size)
	{
		int32 ret = 0;

		// initialize parent
		ret = Hash::init(hash_size);
		if (ret == FAILURE)
			return FAILURE;

		// initilize mutex
		ret = _mutex.init();
		if (ret == FAILURE)
			return FAILURE;

		// ok
		setOk(true);
		return SUCCESS;
	}

	char* SharedHash::get(char* key, // key to get
	uint32 key_size, // key size in bytes
	bool try_lock   	   // whether to lock or try a lock
	)
	{
		char* val = NULL;

		if (try_lock) {
			if (_mutex.tryLock() == FAILURE)
				return NULL;
		} else
			_mutex.lock();

		val = Hash::get(key, key_size);
		_mutex.unlock();

		return val;
	}

	char* SharedHash::get(char* key, // key to get
	uint32 key_size, // key size in bytes
	uint32* val_size, // value size in bytes
	bool try_lock   	   // whether to lock or try a lock
	)
	{
		char* val = NULL;

		if (try_lock) {
			if (_mutex.tryLock() == FAILURE)
				return NULL;
		} else
			_mutex.lock();

		val = Hash::get(key, key_size, val_size);
		_mutex.unlock();

		return val;
	}

	int32 SharedHash::put(char* key, // key to put
	uint32 key_size, // key size in bytes
	char* val, // value to put (cast to char* may be needed)
	uint32 val_size, // value size in bytes
	bool replace, // whether to replace an existing key
	bool try_lock   		// whether to lock or try a lock
	)
	{
		int32 ret;

		if (try_lock) {
			if (_mutex.tryLock() == FAILURE)
				return HASH_BUSY;
		} else
			_mutex.lock();

		ret = Hash::put(key, key_size, val, val_size, replace);
		_mutex.unlock();

		return ret;
	}

	int32 SharedHash::pput(char* key, uint32 key_size, char* val,
		bool replace, bool try_lock)
	{
		int32 ret;

		if (try_lock) {
			if (_mutex.tryLock() == FAILURE)
				return HASH_BUSY;
		} else
			_mutex.lock();

		ret = Hash::pput(key, key_size, val, replace);
		_mutex.unlock();

		return ret;
	}

	int32 SharedHash::del(char* key, // key to remove
	uint32 key_size, // key size in bytes
	bool try_lock   	   // whether to lock or try a lock
	)
	{
		int32 ret;

		if (try_lock) {
			if (_mutex.tryLock() == FAILURE)
				return HASH_BUSY;
		} else
			_mutex.lock();

		ret = Hash::del(key, key_size);
		_mutex.unlock();

		return ret;
	}
}

