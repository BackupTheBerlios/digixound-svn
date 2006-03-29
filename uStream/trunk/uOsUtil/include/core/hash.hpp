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

#ifndef HASH_HPP
#define HASH_HPP

#include "enum.hpp"
#include "databuf.hpp"

namespace uStreamLib {
	class US_API_EXPORT Hash : public Object {
	public:
		/**
		 * Hash table's specific error codes.
		 */
		enum {
		/**
		 * The key already exists in the table.
		 */
		KEY_EXISTS = 10, 
		/**
		 * The key was not found.
		 */
		KEY_NOT_FOUND = 11 };

		/**
		 * Constructor.
		 */
		Hash(void);

		/**
		 * Destructor.
		 */
		virtual ~Hash(void);

		/**
		 * Build an hash table.
		 * @param size hash buckets. Please, specify a number that is
		 * far from a power of 2 and possible prime.
		 * @return SUCCESS or FAILURE if out of memory.
		 */
		int32 init(uint32 size);

		/**
		 * Compute hash code.
		 * This method is invoked when an hash code is needed.
		 * @param key a pointer to the key buffer.
		 * @param key_size key size in bytes.
		 * @param hash_size number of buckets in the hash table.
		 * @return an hash code.
		 */
		virtual uint32 hashCode(char* key, uint32 key_size, uint32 hash_size);

		/**
		 * Compare keys.
		 * This method is invoked when a comparison between keys is needed.
		 */
		virtual int32 compare(char* a, char* b, uint32 a_b_size);

		/**
		 * Print statistics on standard output.
		 */
		void statistics(void);

		/**
		 * Get current hash size.
		 */
		uint32 getSize(void)
		{
			return _size;
		}

		/**
		 * Get current elements count.
		 */
		uint32 getCount(void)
		{
			return _elem_count;
		}

		/**
		 * Get an item given its key.
		 */
		char* get(char* key, uint32 key_size);

		/**
		 * Get an item and its size, given its key.
		 */
		char* get(char* key, uint32 key_size, uint32* val_size);

		/**
		 * Get an item and its size, given its key which is assumed
		 * to be a null terminated string.
		 */
		char* get(char* key, uint32* val_size)
		{
			return get(key, (uint32) strlen(key) + 1, val_size);
		}

		/**
		 * Get an item given its key which is assumed to be a null
		 * terminated string.
		 */
		char* get(char* key)
		{
			return get(key, (uint32) strlen(key) + 1);
		}

		int32 put(char* key, uint32 key_size, char* val, uint32 val_size,
			bool replace = false);

		int32 pput(char* key, uint32 key_size, char* val, bool replace = false);

		int32 put(char* key, char* val, uint32 val_size, bool replace = false)
		{
			return put(key, (uint32) strlen(key) + 1, val, val_size, replace);
		}

		int32 pput(char* key, char* val, bool replace = false)
		{
			return pput(key, (uint32) strlen(key) + 1, val, replace);
		}

		int32 put(char* key, char* val, bool replace = false)
		{
			return put(key, (uint32) strlen(key) + 1, val,
					(uint32) strlen(val) + 1, replace);
		}

		int32 del(char* key, uint32 key_size);

		int32 del(char* key)
		{
			return del(key, (uint32) strlen(key) + 1);
		}

		Enumeration* keys(void);
		Enumeration* values(void);

		int32 saveKV(char* filename);
		int32 loadKV(char* filename);

		void clear(void);

		char* getErrorString(void)
		{
			return _dbErrorString.toString();
		}
	private:
		/* copy constructor not available */
		Hash(Hash&)
			: Object(UOSUTIL_RTTI_HASH)
		{
		}

		/* hash element */
		struct hash_elem_t {
			/* ptr to the key */
			char* key;

			/* key size */
			uint32 key_size;

			/* related value (memcopied) */
			char* value;

			/* value size */
			uint32 value_size;

			/* related value (not memcopied) */
			char* pvalue;

			/* collision chain */
			struct hash_elem_t* next;

			/* empty marker */
			bool empty;

			/* collision chain length */
			uint32 hit_count;
		}
		* _hte;

		/* number of buckets */
		uint32 _size;

		/* number of elements */
		uint32 _elem_count;

		/* total allocated bytes */
		uint32 _bytes;

		/* keys enumeration */
		Enumeration _enKeys;

		/* values enumeration */
		Enumeration _enValues;

		/* buffer for storing error message */
		DataBuf _dbErrorString;

		/* method to delete a hash_elem */
		void _htefree(struct hash_elem_t* hte);
	};
}

#endif
