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

#include "formats_byte.hpp"

namespace uStreamLib {
	ByteFormats::ByteFormats(void)
	{
		setClassID(UOSUTIL_RTTI_BYTE_FORMATS);
	}

	ByteFormats::~ByteFormats(void)
	{
		// nothing to do
	}

	int32 ByteFormats::init(uint32 hsize)
	{
		return SharedHash::init(hsize);
	}

	char* ByteFormats::getByteFormatString(ByteStreamFormat bf)
	{
		switch (bf) {
		case BF_UNDEF:
			return "BF_UNDEFINED";
		case BF_AVI:
			return "BF_AVI";
		case BF_DNF:
			return "BF_DNF";
		case BF_DAF:
			return "BF_DAF";
		case BF_DTF:
			return "BF_DTF";
		case BF_DAVT:
			return "BF_DAVT";
		case BF_WAV:
			return "BF_WAV";
		case BF_ISL_OVER_BIC:
			return "BF_ISL_OVER_BIC";
		case BF_FLOAT_STREAM:
			return "BF_FLOAT_STREAM";
		case BF_DOUBLE_STREAM:
			return "BF_DOUBLE_STREAM";
		case BF_INT32_STREAM:
			return "BF_INT32_STREAM";
		case BF_UINT32_STREAM:
			return "BF_UINT32_STREAM";
		case BF_INT16_STREAM:
			return "BF_INT16_STREAM";
		case BF_UINT16_STREAM:
			return "BF_UINT16_STREAM";
		case BF_BYTES:
			return "BF_BYTES_STREAM";
		}

		return "UNDEFINED";
	}
}
