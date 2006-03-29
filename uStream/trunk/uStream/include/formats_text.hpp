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

#ifndef TEXTFORMATS_HPP
#define TEXTFORMATS_HPP

#include "shared_hash.hpp"
#include "vector.hpp"
#include "types.hpp"

namespace uStreamLib {
	/**
	 * This class allows to manage text formats and encodings.
	 * It can fill a resolution vector with strings indicating the
	 * specified text format and encoding.
	 */
	class US_EXPORT TextFormats : public SharedHash {
	public:
		/**
		 * Constructor.
		 */
		TextFormats(void);

		/**
		 * Destructor.
		 */
		virtual ~TextFormats(void);

		/**
		 * Build a text formats container.
		 */
		int32 init(uint32 hsize = 39);

		/**
		 * Method to fill a vector with text format strings.
		 */
		void fillVector(Vector* v);

		/**
		 * Method to get a string rapresentation of text format.
		 * @param vf text format.
		 * @return a string form of the specified text format.
		 */
		static char* getTextFormatString(TextFormat tf);
	private:
		/* copy constructor not available */
		TextFormats(TextFormats&)
		{
		};
	};
}

#endif
