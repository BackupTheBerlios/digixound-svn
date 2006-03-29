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
#include <stdarg.h>
#include <time.h>

#include "typedefs.hpp"
#include "utils.hpp"
#include "vector.hpp"

namespace uStreamLib {
	/*
	 * Methods.
	 */
	int32 Utils::getDelim(char* line, uint32 size, char delim, FILE* fp)
	{
		register uint32 count = 0;
		char c, next = 0;

		do {
			c = (char) fgetc(fp);
			if (c == EOF)
				break;

			if (c == '\\') {
				next = (char) fgetc(fp);
				if (next == delim) {
					continue;
				} else {
					ungetc(next, fp);
				}
			}

			line[count++] = c;

			if (count > size) {
				line[count - 2] = delim;
				line[count - 1] = 0;
				return size;
			}

			if (c == delim) {
				line[count] = 0;
				return count;
			}
		} while (1);

		return FAILURE;
	}

	char* Utils::getToken(char** cur, char* delim, uint32 cursz,
		uint32 delimsz)
	{
		register char * dlm = delim;
		register char * start = *cur, * str = *cur;

		if (!cur || !delim)
			return NULL;
		if (!*cur)
			return NULL;

		do {
			dlm = delim;

			do {
				if (*str == *dlm) {
					*str = 0;
					*cur = ++str;

					return start;
				}
			} while (*(++dlm));
		} while (*(str++));

		*cur = NULL;
		return start;
	}

	char* Utils::int32ToString(int32 value, char* out, uint32 out_sz)
	{
		snprintf(out, out_sz, "%d", value);
		return out;
	}

	char* Utils::uint32ToString(uint32 value, char* out, uint32 out_sz)
	{
		snprintf(out, out_sz, "%d", value);
		return out;
	}

	char* Utils::int64ToString(int64 value, char* out, uint32 out_sz)
	{
#if defined(WIN32) || defined(_WIN32)
		return _i64toa(value, out, 10);
#else
		snprintf(out, out_sz, "%lld", value);
		return out;
#endif
	}

	char* Utils::uint64ToString(uint64 value, char* out, uint32 out_sz)
	{
#if defined(WIN32) || defined(_WIN32)
		return _ui64toa(value, out, 10);
#else
		snprintf(out, out_sz, "%llu", value);
		return out;
#endif
	}

	int32 Utils::stringToInt32(char* in)
	{
		return ::strtol(in, NULL, 10);
	}

	int64 Utils::stringToInt64(char* in)
	{
#if defined(WIN32) || defined(_WIN32)
		return _atoi64(in);
#else
		return strtoll(in, NULL, 10);
#endif
	}

	uint32 Utils::stringToUint32(char* in)
	{
		return ::strtoul(in, NULL, 10);
	}

	uint64 Utils::stringToUint64(char* in)
	{
#if defined(WIN32) || defined(_WIN32)
		return (uint64) _atoi64(in);
#else
		return strtoull(in, NULL, 0);
#endif
	}
}
