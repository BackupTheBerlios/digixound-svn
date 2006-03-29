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

#include "formats_text.hpp"

namespace uStreamLib {
	TextFormats::TextFormats(void)
	{
		setClassID(UOSUTIL_RTTI_TEXT_FORMATS);
	}

	TextFormats::~TextFormats(void)
	{
		// nothing to do
	}

	int32 TextFormats::init(uint32 hsize)
	{
		return SharedHash::init(hsize);
	}

	char* TextFormats::getTextFormatString(TextFormat tf)
	{
		switch (tf) {
		case TF_UNDEF:
			return "TF_UNDEF";
		case TF_ASCII_ISL:
			return "TF_ASCII_ISL";
		case TF_ASCII_KV:
			return "TF_ASCII_KV";
		case TF_ASCII_CMSL:
			return "TF_ASCII_CMSL";
		case TF_XML:
			return "TF_XML";
		}

		return "UNDEFINED";
	}
}
