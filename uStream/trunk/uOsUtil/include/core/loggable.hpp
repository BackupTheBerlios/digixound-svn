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

#ifndef LOGGABLE_HPP
#define LOGGABLE_HPP

#include "logger.hpp"

namespace uStreamLib {
	/**
	 * This class is a loggable.
	 *
	 * Any class that needs to log messages in some way can inherit
	 * from this class and then invoke the log() method.
	 */
	class US_API_EXPORT Loggable : public Object {
	public:
		/**
		 * Constructor.
		 */
		Loggable(void);

		/**
		 * Destructor.
		 */
		virtual ~Loggable(void);

		/**
		 * Set pointer to logger.
		 */
		virtual void setLogger(Logger* value)
		{
			m_logger = value;
		}

		/**
		 * Get pointer to logger.
		 */
		virtual Logger* getLogger(void)
		{
			return m_logger;
		}

		/**
		 * Log specified message to default log.
		 * If no logger is set, then output is on standard error.
		 */
		void log(uint8 level, char* fmt, ...);

		/**
		 * Log specified message to specified log.
		 * If specified logger is not found, default logger will be used.
		 * If no logger is set, then output is on standard error.
		 */
		void log(char* logger, uint8 level, char* ftm, ...);
	private:
		/* pointer to logger */
		Logger* m_logger;
	};
}

#endif
