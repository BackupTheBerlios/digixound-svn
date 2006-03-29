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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.hpp"
#include "utils.hpp"
#include "memory.hpp"

namespace uStreamLib {
	Hash::Hash(void)
		: Object(UOSUTIL_RTTI_HASH), _hte(NULL), _size(0), _elem_count(0),
		_bytes(0)
	{
		// nothing to do
	}

	Hash::~Hash(void)
	{
		struct hash_elem_t* cur = NULL;
		struct hash_elem_t* base = NULL;
		struct hash_elem_t* next = NULL;
		uint32 i = 0;

		for (i = 0; i < _size; i++) {
			// get a base slot
			base = &(_hte[i]);
			cur = base->next;

			// scan its chain
			while (cur) {
				// store next element
				next = cur->next;

				// remove current element
				if (!cur->empty)
					_htefree(cur);

				free(cur);
				_bytes -= sizeof(struct hash_elem_t);

				// update cursor
				cur = next;
			}

			if (!base->empty)
				_htefree(base);
		}

		if (_hte)
			free(_hte);
	}

	int32 Hash::init(uint32 size)
	{
		uint32 i, chain_depth = 8;
		int32 ret = 0;

		ret = _dbErrorString.init(16);
		if (ret == FAILURE)
			return FAILURE;

		_hte = (struct hash_elem_t *)
			malloc(sizeof(struct hash_elem_t) * size);
		if (!_hte)
			return FAILURE;

		// DEBUG
		UOSUTIL_DOUT(("Hash(): init(%u)\n", size));

		_size = size;
		_elem_count = 0;
		_bytes = 0;

		ret = _enKeys.init(size * chain_depth);
		if (ret == FAILURE)
			return FAILURE;

		ret = _enValues.init(size * chain_depth);
		if (ret == FAILURE)
			return FAILURE;

		for (i = 0; i < _size; i++) {
			_hte[i].empty = true;
			_hte[i].key = NULL;
			_hte[i].key_size = 0;
			_hte[i].value = NULL;
			_hte[i].pvalue = NULL;
			_hte[i].value_size = 0;
			_hte[i].next = NULL;
			_hte[i].hit_count = 0;
		}

		setOk(true);
		return SUCCESS;
	}

	uint32 Hash::hashCode(char* key, uint32 key_size, uint32 hash_size)
	{
		uint32 code = 0, g = 0;
		char* ptr = key;

		if (!hash_size)
			return 0;

		for (uint32 i = 0; i < key_size; i++, ptr++) {
			code = (code << 4) + *ptr;
			if ((g = code & 0xf0000000)) {
				code ^= (g >> 24);
				code ^= g;
			}
		}

		return (code % hash_size);
	}

	int32 Hash::compare(char* a, char* b, uint32 a_b_size)
	{
		return memcmp(a, b, a_b_size);
	}

	int32 Hash::put(char* key, uint32 keysz, char* val, uint32 valsz,
		bool replace)
	{
		struct hash_elem_t* cur = NULL;
		struct hash_elem_t* pre = NULL;
		uint32 code = 0, do_replace = 0;

		/* compute hash code */
		if (!_size)
			return FAILURE;
		code = hashCode(key, keysz, _size);

		/* no collision detected */
		if (_hte[code].empty) {
			_hte[code].key = (char *) malloc(keysz);
			if (!_hte[code].key)
				return FAILURE;

			Memory::memCopy(_hte[code].key, key, keysz);
			_hte[code].key_size = keysz;
			_bytes += keysz;

			_hte[code].value = (char *) malloc(valsz);
			if (!_hte[code].value) {
				free(_hte[code].key);
				return FAILURE;
			}

			Memory::memCopy(_hte[code].value, val, valsz);
			_hte[code].value_size = valsz;
			_hte[code].empty = false;
			_elem_count++;
			_bytes += valsz;

			return SUCCESS;
		}

		/* collision detected */
		cur = &_hte[code];

		/* get statistics on collisions */
		if (!replace)
			cur->hit_count += 1;

		/* chain scanning */
		while (!cur->empty) {
			/* check for key duplicates */
			if (cur->key_size == keysz) {
				if (!compare(cur->key, key, keysz)) {
					if (replace) {
						do_replace = 1; break;
					} else
						return KEY_EXISTS;
				}
			}

			// DEBUG
			UOSUTIL_DOUT(("Hash::put(%s [%u]): collision, code = %u, hit = %u\n",
				key, keysz, code, cur->hit_count));

			/* update pointers */
			pre = cur;
			cur = cur->next;

			if (cur == NULL) {
				cur = (struct hash_elem_t *)
					malloc(sizeof(struct hash_elem_t));
				if (!cur)
					return FAILURE;

				memset(cur, 0, sizeof(struct hash_elem_t));
				_bytes += sizeof(struct hash_elem_t);

				cur->empty = true;
				pre->next = cur;

				break;
			}
		}

		if (do_replace) {
			if (valsz > cur->value_size) {
				cur->value = (char *) realloc(cur->value, valsz);
				if (!cur->value)
					return FAILURE;
			}

			_bytes += (valsz - cur->value_size);

			Memory::memCopy(cur->value, val, valsz);
			cur->value_size = valsz;
		} else {
			cur->key = (char *) malloc(keysz);
			if (!cur->key)
				return FAILURE;

			Memory::memCopy(cur->key, key, keysz);
			cur->key_size = keysz;

			_bytes += keysz;

			cur->value = (char *) malloc(valsz);
			if (!cur->value) {
				free(cur->key);
				return FAILURE;
			}

			Memory::memCopy(cur->value, val, valsz);
			cur->value_size = valsz;

			_bytes += valsz;

			cur->empty = false;
			cur->next = NULL;

			_elem_count++;
		}

		return SUCCESS;
	}

	int32 Hash::pput(char* key, uint32 keysz, char* val, bool replace)
	{
		struct hash_elem_t* cur = NULL;
		struct hash_elem_t* pre = NULL;
		uint32 code = 0, do_replace = 0;

		/* compute hash code */
		if (!_size)
			return FAILURE;
		code = hashCode(key, keysz, _size);

		/* no collision detected */
		if (_hte[code].empty) {
			_hte[code].key = (char *) malloc(keysz);
			if (!_hte[code].key)
				return FAILURE;

			Memory::memCopy(_hte[code].key, key, keysz);
			_hte[code].key_size = keysz;

			_bytes += keysz;

			_hte[code].value = NULL;
			_hte[code].value_size = 0;
			_hte[code].pvalue = val;
			_hte[code].empty = false;

			_elem_count++;
			return SUCCESS;
		}

		/* collision detected */
		cur = &(_hte[code]);

		/* get statistics on collisions */
		if (!replace)
			cur->hit_count++;

		/* chain scanning */
		while (!cur->empty) {
			/* check for key duplicates */
			if (cur->key_size == keysz) {
				if (!compare(cur->key, key, keysz)) {
					if (replace) {
						do_replace = 1; break;
					} else
						return KEY_EXISTS;
				}
			}

			// DEBUG
			UOSUTIL_DOUT(("Hash::pput(%s [%u]): collision, code = %u, hit = %u\n",
				key, keysz, code, cur->hit_count));

			/* update pointers */
			pre = cur;
			cur = cur->next;

			if (cur == NULL) {
				cur = (struct hash_elem_t *)
					malloc(sizeof(struct hash_elem_t));
				if (!cur)
					return FAILURE;

				memset(cur, 0, sizeof(struct hash_elem_t));
				_bytes += sizeof(struct hash_elem_t);

				cur->empty = true;
				pre->next = cur;

				break;
			}
		}

		if (do_replace) {
			cur->pvalue = val;
		} else {
			cur->key = (char *) malloc(keysz);
			if (!cur->key)
				return FAILURE;

			Memory::memCopy(cur->key, key, keysz);
			cur->key_size = keysz;

			_bytes += keysz;

			cur->value = NULL;
			cur->value_size = 0;
			cur->pvalue = val;
			cur->empty = false;
			cur->next = NULL;

			_elem_count++;
		}

		return SUCCESS;
	}

	char* Hash::get(char* key, uint32 key_size)
	{
		struct hash_elem_t* cur = NULL;
		uint32 code = 0, depth = 0;

		if (!_size)
			return NULL;
		code = hashCode(key, key_size, _size);

		/* direct access */
		if (_hte[code].key_size == key_size) {
			if (!compare(_hte[code].key, key, key_size)) {
				if (_hte[code].value)
					return _hte[code].value;
				else
					return _hte[code].pvalue;
			}
		}

		/* chain scanning */
		cur = &(_hte[code]);

		while (cur->next) {
			cur = cur->next;

			if (cur->key_size == key_size) {
				if (!compare(cur->key, key, key_size)) {
					if (cur->value) {
						UOSUTIL_DOUT(("Hash::get(%s [%u]): depth = %u\n", key,
							key_size, depth));
						return cur->value;
					} else {
						UOSUTIL_DOUT(("Hash::get(%s [%u]): depth = %u\n", key,
							key_size, depth));
						return cur->pvalue;
					}
				}
			}

			depth += 1;
		}

		/* key not found */
		return NULL;
	}

	char* Hash::get(char* key, uint32 key_size, uint32* valsz)
	{
		struct hash_elem_t* cur = NULL;
		uint32 code = 0, depth = 0;

		if (!_size)
			return NULL;
		code = hashCode(key, key_size, _size);

		/* direct access */
		if (_hte[code].key_size == key_size) {
			if (!compare(_hte[code].key, key, key_size)) {
				*valsz = _hte[code].value_size;
				if (_hte[code].value)
					return _hte[code].value;
				else
					return _hte[code].pvalue;
			}
		}

		/* chain scanning */
		cur = &(_hte[code]);

		while (cur->next) {
			cur = cur->next;

			if (cur->key_size == key_size) {
				if (!compare(cur->key, key, key_size)) {
					*valsz = cur->value_size;

					if (cur->value) {
						UOSUTIL_DOUT(("Hash::get(%s [%u]): depth = %u\n", key,
							key_size, depth));
						return cur->value;
					} else {
						UOSUTIL_DOUT(("Hash::get(%s [%u]): depth = %u\n", key,
							key_size, depth));
						return cur->pvalue;
					}
				}
			}

			depth += 1;
		}

		/* key not found */
		*valsz = 0;
		return NULL;
	}

	int32 Hash::del(char* key, uint32 key_size)
	{
		struct hash_elem_t* cur = NULL;
		struct hash_elem_t* base = NULL;
		uint32 code = 0;

		if (!_size)
			return FAILURE;
		code = hashCode(key, key_size, _size);

		/* direct access */
		if (_hte[code].key_size == key_size) {
			if (!compare(_hte[code].key, key, key_size)) {
				_htefree(&_hte[code]);
				_elem_count--;
				return SUCCESS;
			}
		}

		/* chain scanning */
		base = &(_hte[code]);
		cur = base;

		while (cur->next) {
			cur = cur->next;
			if (cur->key_size == key_size) {
				if (!compare(cur->key, key, key_size)) {
					_htefree(cur);

					_elem_count -= 1;
					base->hit_count -= 1;

					return SUCCESS;
				}
			}
		}

		/* key not found */
		return FAILURE;
	}

	Enumeration* Hash::keys(void)
	{
		struct hash_elem_t* cur;
		struct hash_elem_t* base;
		uint32 i;

		_enKeys.clear();

		for (i = 0; i < _size; i++) {
			base = &(_hte[i]);
			cur = base;

			if (!cur->empty) {
				_enKeys.addElement(base->key, base->key_size);
			}

			while (cur->next != NULL) {
				cur = cur->next;
				if (!cur->empty) {
					_enKeys.addElement(cur->key, cur->key_size);
				}
			}
		}

		_enKeys.rewind();

		// ok
		return &_enKeys;
	}

	Enumeration* Hash::values(void)
	{
		struct hash_elem_t* cur;
		struct hash_elem_t* base;
		uint32 i;

		_enValues.clear();

		for (i = 0; i < _size; i++) {
			base = &(_hte[i]);
			cur = base;

			if (!cur->empty) {
				if (base->value)
					_enValues.addElement(base->value, base->value_size);
				else
					_enValues.addElement(base->pvalue, base->value_size);
			}

			while (cur->next != NULL) {
				cur = cur->next;
				if (!cur->empty) {
					if (cur->value)
						_enValues.addElement(cur->value, cur->value_size);
					else
						_enValues.addElement(cur->pvalue, cur->value_size);
				}
			}
		}

		_enValues.rewind();

		// ok
		return &_enValues;
	}

	void Hash::clear(void)
	{
		Enumeration* en = keys();
		while (en->hasMoreElements()) {
			char* key = (char*) en->nextElement();
			UOSUTIL_DOUT(("Hash::clear(): deleting key = \"%s\"\n", key));

			del(key);
		}
	}

	void Hash::statistics(void)
	{
		uint32 i;

		printf("Hash::statistics: TOTAL %u\n", _elem_count);
		printf("Hash::statistics: TOTAL BYTES %u\n", _bytes);
		for (i = 0; i < _size; i++)
			printf("%u %u\n", i, _hte[i].hit_count);
	}

	int32 Hash::loadKV(char* filename)
	{
		FILE* fp = NULL;

		char line[4096];
		char* line_ptr = NULL, * end_ptr = NULL;
		char* cur_ptr = NULL, * key = NULL, * val = NULL;
		int32 ret = 0, error = 0, line_count = 0;

		// open the specified file for reading
		fp = fopen(filename, "r");
		if (!fp) {
			_dbErrorString.xcopy("open error"); return FAILURE;
		}

		while (1) {
			// count lines
			line_count++;

			// read one line
			ret = Utils::getDelim(line, 4096, LF, fp);
			if (ret < 0)
				break;

			// remove ending spaces and line terminators
			line_ptr = line;
			end_ptr = &line[ret - 1];
			while (end_ptr >=
				line_ptr &&
				(*end_ptr == ' ' ||
				*end_ptr == '\t' ||
				*end_ptr == LF ||
				*end_ptr == CR)) {
				*(end_ptr--) = '\0';
			}

			// remove trailing spaces
			while (*line_ptr != '\0' &&
				(*line_ptr == ' ' || *line_ptr == '\t'))
				line_ptr++;

			// ignore empty lines and comments
			if (*line_ptr == '\0' || *line_ptr == '#')
				continue;

			// do tokenizing
			cur_ptr = line_ptr;

			key = Utils::getToken(&cur_ptr, "=");
			if (key == NULL)
				continue;

			val = cur_ptr;
			if (val == NULL)
				val = "";

			// do trimming of key and val
			while (*val != '\0' && (*val == ' ' || *val == '\t'))
				val++;

			ret = put(key, val, (uint32) strlen(val) + 1);
			if (ret == FAILURE) {
				error = 2; break;
			}
		}

		// close the file
		fclose(fp);

		// detect error condition
		if (error == 2) {
			_dbErrorString.xcopy("out of memory"); return FAILURE;
		}

		// everything ok
		return SUCCESS;
	}

	int32 Hash::saveKV(char* filename)
	{
		Enumeration* ek = NULL;
		FILE* fp = NULL;

		char* key = NULL, * val = NULL;
		int32 ret = 0, error = 0;

		// open the specified file for writing
		fp = fopen(filename, "w+");
		if (!fp) {
			_dbErrorString.xcopy("open error"); return FAILURE;
		}

		// take all the keys
		ek = keys();
		while (ek->hasMoreElements()) {
			key = (char *) ek->nextElement();

			val = (char *) get(key);
			if (!val) {
				error = 1; break;
			}

			ret = fprintf(fp, "%s=%s\n", key, val);
			if (ret < 0) {
				error = 2; break;
			}
		}

		// close the file
		fclose(fp);

		// detect error conditions
		if (error == 1) {
			_dbErrorString.xcopy("null value"); return FAILURE;
		}
		if (error == 2) {
			_dbErrorString.xcopy("write error"); return FAILURE;
		}

		// everything ok
		return SUCCESS;
	}

	void Hash::_htefree(struct hash_elem_t* hte)
	{
		if (hte->value)
			free(hte->value);
		if (hte->key)
			free(hte->key);

		_bytes -= (hte->key_size + hte->value_size);

		hte->key = NULL;
		hte->value = NULL;
		hte->pvalue = NULL;
		hte->key_size = 0;
		hte->value_size = 0;
		hte->empty = true;
		hte->hit_count = 0;
	}
}
