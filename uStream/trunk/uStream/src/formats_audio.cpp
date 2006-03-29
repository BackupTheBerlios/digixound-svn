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

#include "formats_audio.hpp"

namespace uStreamLib {
	AudioFormats::AudioFormats(void)
	{
		setClassID(UOSUTIL_RTTI_AUDIO_FORMATS);
	}

	AudioFormats::~AudioFormats(void)
	{
		// nothing to do  
	}

	int32 AudioFormats::init(uint32 hsize)
	{
		return SharedHash::init(hsize);
	}

	int32 AudioFormats::build(audioformat* user_fmts)
	{
		const int32 entries_count = 18;
		int32 ret = 0;

		// format strings (18 entries)
		char* afmt_str[] = {
			"AF_UNDEF", "AF_U8", "AF_S8", "AF_S16_LE", "AF_S16_BE",
			"AF_S16_NE", "AF_S32_LE", "AF_S32_BE", "AF_U16_LE", "AF_U16_BE",
			"AF_MLAW", "AF_ALAW", "AF_ADPCM", "AF_S16_CH1", "AF_S16_CH2",
			"AF_S16_CH4", "AF_S16_CH5", "AF_S16_CH6"
		};

		// default formats (not so good)
		audioformat afmt[] = {
			{0,    0,  0, 0, 1,   AF_UNDEF}, {0, 8000,  8, 1, 1, 	 AF_U8},
			{0, 8000,  8, 1, 1, 	 AF_S8}, {0, 8000,  8, 1, 1,  AF_S16_LE},
			{0, 8000, 16, 1, 0,  AF_S16_BE}, {0, 8000, 16, 1, 0,  AF_S16_NE},
			{0, 8000, 16, 1, 1,  AF_S32_LE}, {0, 8000, 16, 1, 0,  AF_S32_BE},
			{0, 8000, 16, 1, 1,  AF_U16_LE}, {0, 8000, 16, 1, 0,  AF_U16_BE},
			{0, 8000, 16, 1, 1,    AF_MLAW}, {0, 8000, 16, 1, 1,	AF_ALAW},
			{0, 8000, 16, 1, 1,   AF_ADPCM}, {0, 8000, 16, 1, 1, AF_S16_CH1},
			{0, 8000, 16, 2, 1, AF_S16_CH2}, {0, 8000, 16, 4, 1, AF_S16_CH4},
			{0, 8000, 16, 5, 1, AF_S16_CH5}, {0, 8000, 16, 6, 1, AF_S16_CH6}
		};

		if (user_fmts) {
			for (int32 i = 0; i < entries_count; i++) {
				ret = put(afmt_str[i], (char *) &user_fmts[i],
						sizeof(audioformat));
				if (ret == FAILURE)
					return FAILURE;
			}
		} else {
			for (int32 i = 0; i < entries_count; i++) {
				ret = put(afmt_str[i], (char *) &afmt[i], sizeof(audioformat));
				if (ret == FAILURE)
					return FAILURE;
			}
		}

		return SUCCESS;
	}

	void AudioFormats::fillVector(Vector* v)
	{
		Enumeration* en = keys();
		while (en->hasMoreElements()) {
			char* key = (char*) en->nextElement();
			v->addElement(key, (uint32) strlen(key) + 1);
		}
	}

	char* AudioFormats::getAudioFormatString(AudioFormat af)
	{
		switch (af) {
		case AF_UNDEF:
			return "AF_UNDEF";
		case AF_U8:
			return "AF_U8";
		case AF_S8:
			return "AF_S8";
		case AF_S16_LE:
			return "AF_S16_LE";
		case AF_S16_BE:
			return "AF_S16_BE";
		case AF_S16_NE:
			return "AF_S16_NE";
		case AF_S32_LE:
			return "AF_S32_LE";
		case AF_S32_BE:
			return "AF_S32_BE";
		case AF_U16_LE:
			return "AF_U16_LE";
		case AF_U16_BE:
			return "AF_U16_BE";
		case AF_MLAW:
			return "AF_MLAW";
		case AF_ALAW:
			return "AF_ALAW";
		case AF_ADPCM:
			return "AF_ADPCM";
		case AF_S16_CH1:
			return "AF_S16_CH1";
		case AF_S16_CH2:
			return "AF_S16_CH2";
		case AF_S16_CH4:
			return "AF_S16_CH4";
		case AF_S16_CH5:
			return "AF_S16_CH5";
		case AF_S16_CH6:
			return "AF_S16_CH6";
		}

		return "UNDEFINED";
	}
}
