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

#ifndef CALLBACK_HPP
#define CALLBACK_HPP

#include "databuf.hpp"

namespace uStreamLib {
	/**
	 * A callback class.
	 * This class is used to create callbacks for Interp and
	 * other entities which needs to execute specific code when
	 * an event occurs. We can call it a "function class".
	 */
	class US_API_EXPORT CallBack : public Object {
	public:
		/**
		 * Constructor.
		 */
		CallBack(void);

		/**
		 * Destructor.
		 */
		virtual ~CallBack(void);

		/**
		 * Initialize a callback object.
		 * @param name name of the callback (useful for debug and more).
		 * @return SUCCESS or FAILURE.
		 */
		int32 init(char* name);

		/**
		 * Get this callback's name.
		 * @return callback's name.
		 */
		char* getName()
		{
			return _dbName.toString();
		}

		/**
		 * This method is the entry point of a callback.
		 * @param custom_data generic data pointer for user provided data.
		 */
		virtual int32 perform(void*)
		{
			return FAILURE;
		}
	private:
		/* copy constructor not avilable */
		CallBack(CallBack&)
			: Object(UOSUTIL_RTTI_CALLBACK)
		{
		}

		/* callback's name */
		DataBuf _dbName;
	};
}

#endif
