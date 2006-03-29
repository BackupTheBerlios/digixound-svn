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

#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#ifdef HAVE_CONFIG_H
# include "global.h"
#else
# define VERSION "5.0.1-01"
#endif

/*
 * This is the operating system specific include file.
 * I think we avoid redefinition errors for windows.h
 * and winsock2.h inclusion.
 */
#include "osincludes.hpp"

/*
 * Include basic enums.
 */
#include "core_enums.hpp"

/*
 * Define basic types.
 */

#if defined(WIN32) || defined(_WIN32) || defined(US_POSIX_API)
# include <sys/types.h>
#endif

namespace uStreamLib {
#if defined(_WIN32) || defined(WIN32)

	typedef unsigned __int64 uint64;
	typedef __int64 int64;

#else

	typedef unsigned long long int uint64;
	typedef signed long long int int64;

#endif

	typedef unsigned int uint32;
	typedef int int32;

	typedef unsigned short uint16;
	typedef short int16;

	typedef unsigned char uint8;
	typedef signed char int8;
}

#if defined(_WIN32) || defined(WIN32)

/*
 * Disables VisualC++ warning on Exception and
 * performance in bool->integer conversion
 */
# pragma warning ( disable : 4290 )
# pragma warning ( disable : 4800 )

/*
 * Define specific and useful functions.
 */
# define snprintf  _snprintf
# define vsnprintf _vsnprintf

#endif

#define SUCCESS  0
#define FAILURE -1

/*
 * Endian define (used only for testing).
 * This macro must be defined after a
 * detection of endianness.
 */
#define US_LITTLE_ENDIAN

/*
 * Windows specific stuff and portability
 */
#if defined(_WIN32) || defined(WIN32)
# define US_SLEEP_SEC *1000
# define US_FILE_SEP   "\\"
# define US_FILE_SEP_C '\\'

# if defined(LIBUS_CORE_EXPORTS)
#  define US_API_EXPORT __declspec(dllexport)
# else
#  define US_API_EXPORT __declspec(dllimport)
# endif

#else
# define US_SLEEP_SEC
# define US_FILE_SEP   "/"
# define US_FILE_SEP_C '/'
# define US_API_EXPORT
#endif

/*
 * Some constants
 */
#define CR  	'\r'
#define LF  	'\n'
#define CRLF  "\r\n"
#define CR_S	"\r"
#define LF_S	"\n"

/*
 * Useful macros
 */
#if !defined(_WIN32) && !defined(WIN32)
# if defined(US_BIG_ENDIAN)
#  define MAKEFOURCC( ch0, ch1, ch2, ch3 ) ((uint32)(uint8)(ch3)|((uint32)(uint8)(ch2)<<8)|((uint32)(uint8)(ch1)<<16)|((uint32)(uint8)(ch0)<<24))
# else
#  define MAKEFOURCC( ch0, ch1, ch2, ch3 ) ((uint32)(uint8)(ch0)|((uint32)(uint8)(ch1)<<8)|((uint32)(uint8)(ch2)<<16)|((uint32)(uint8)(ch3)<<24))
# endif
#endif

/*
 * Macros for stdout/stderr debugging.
 */
#if defined(UOSUTIL_DEBUG)
# define UOSUTIL_DOUT(fmt) printf fmt
# define UOSUTIL_DERR(fmt) fprintf fmt
#else
# define UOSUTIL_DOUT(fmt)
# define UOSUTIL_DERR(fmt)
#endif

/*
 * Macros for safe deleting objects and arrays.
 */
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

using namespace uStreamLib;

#endif
