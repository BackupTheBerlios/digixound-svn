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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "typedefs.hpp"

namespace uStreamLib {
	/**
	 * Base class for all UOSUTIL classes.
	 * This class gives basic services to all UOSUTIL classes.
	 * One of the most important service is the error detection
	 * code when something goes wrong in the constructor.
	 */
	class US_API_EXPORT Object {
	public:
		/**
		 * Constructor.
		 * Build an object.
		 * @param class_id class identifier.
		 */
		Object(uint32 class_id);

		/**
		 * Copy constructor.
		 */
		Object(Object&);

		/**
		 * Destructor.
		 */
		virtual ~Object(void);

		/**
		 * Check if this object is valid.
		 * @return true or false.
		 */
		bool isOk(void)
		{
			return _bIsOk;
		}

		/**
		 * Get error code.
		 * @return an unsigned integer.
		 */
		uint32 getErrorCode(void)
		{
			return _uErrorCode;
		}

		/**
		 * Get error string.
		 * This method must be redefined by any child.
		 */
		virtual char* getErrorString(void)
		{
			return (char *) 0;
		}

		/**
		 * Get class identifier.
		 * @return class identifier.
		 */
		uint32 getClassID(void)
		{
			return _uClassID;
		}
		
	protected:
		/**
		 * Signal that this object is not valid.
		 * @param flag true if object's ok, false otherwise.
		 */
		void setOk(bool flag)
		{
			_bIsOk = flag;
		}

		/**
		 * Set error code.
		 * @param code error code.
		 */
		void setErrorCode(uint32 code)
		{
			_uErrorCode = code;
		}

		/**
		 * Set class identifier.
		 * @param class_id class identifier.
		 */
		void setClassID(uint32 class_id)
		{
			_uClassID = class_id;
		}
		
	private:
		/* class identifier */
		uint32 _uClassID;

		/* error code */
		uint32 _uErrorCode;

		/* flag: the object is not valid */
		bool _bIsOk;
	};
}

#endif
