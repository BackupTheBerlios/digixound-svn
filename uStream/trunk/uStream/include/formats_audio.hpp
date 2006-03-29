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

#ifndef AUDIOFORMATS_HPP
#define AUDIOFORMATS_HPP

#include "shared_hash.hpp"
#include "vector.hpp"
#include "types.hpp"

namespace uStreamLib {
	/**
	 * This class manages audio formats by providing methods
	 * to get standard audio device parameters from string
	 * symbols.
	 */
	class US_EXPORT AudioFormats : public SharedHash {
	public:
		/**
		  * This structure defines audio parameters.
		  */
		typedef struct {
			/** hardware/software specific format id */
			int32 format;

			/** sampling rate */
			int32 rate;

			/** bits per sample */
			int8 bits;

			/** number of channels */
			int8 nchans;

			/** flag: is little endian */
			int8 isLE;

			/** uStream specific audio format */
			AudioFormat af;
		} audioformat;

		/**
		 * Build an AudioFormats database.
		 */
		AudioFormats(void);

		/**
		 * Destroy object.
		 */
		virtual ~AudioFormats(void);

		/**
		 * Initialize audio formats object.
		 */
		int32 init(uint32 hsize = 33);

		/**
		 * Build all valid formats.
		 * @param user_fmts list of user provided formats to
		 * associate to format strings. This parameter can be NULL
		 * so that default values are provided.
		 * @return SUCCESS or FAILURE.
		 */
		int32 build(audioformat* user_fmts);

		/**
		 * Fill a vector with know format strings.
		 */
		void fillVector(Vector* v);

		/**
		 * Get an audio format from a string code.
		 * @param fmt_name the format name.
		 * @return a pointer to a audioformat structure.
		 */
		audioformat* getAudioFormat(char* fmt_name)
		{
			return (audioformat *) get(fmt_name);
		}

		/**
		 * Method to get a string rapresentation of audio format.
		 * @param vf audio format.
		 * @return a string form of the specified audio format.
		 */
		static char* getAudioFormatString(AudioFormat af);
	private:
		/* copy constructor not available */
		AudioFormats(AudioFormats&)
		{
		}
	};
}

#endif
