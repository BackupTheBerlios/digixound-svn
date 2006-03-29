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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedefs.hpp"

namespace uStreamLib
{
  class US_API_EXPORT Utils
  {
  public:
    /**
     * Get string up to next delimiter from stdio file.
     */
    static int32 getDelim(char *line,uint32 size,char delim,FILE *fp);

    /**
     * Tokenize string.
     */
    static char *getToken
      (
        char **cur,
        char *delim,
        uint32 cursz = 0,
        uint32 delimsz = 0
      );

    /**
     * Convert 32 bit number to string.
     */
    static char *int32ToString(int32 value,char *out,uint32 out_sz);

    /**
     * Convert 32 bit number to string.
     */
    static char *uint32ToString(uint32 value,char *out,uint32 out_sz);

    /**
     * Convert 64 bit number to string.
     */
    static char *int64ToString(int64 value,char *out,uint32 out_sz);

    /**
     * Convert 64 bit number to string.
     */
    static char *uint64ToString(uint64 value,char *out,uint32 out_sz);

    /**
     * Convert string to 32 bit integer.
     */
    static int32 stringToInt32(char *in);
    
    /**
     * Convert string to 32 bit integer.
     */
    static uint32 stringToUint32(char *in);

    /**
     * Convert string to 64 bit integer.
     */
    static int64 stringToInt64(char *in);

    /**
     * Convert string to 64 bit integer.
     */
    static uint64 stringToUint64(char *in);
  };
}

#endif

