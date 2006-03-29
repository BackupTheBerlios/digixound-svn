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

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "databuf.hpp"
#include "mutex.hpp"

namespace uStreamLib {
	class US_API_EXPORT Logger : public Mutex {
	public:
		/**
		 * Logger modes.
		 */
		enum LoggerMode { 
			/** Write messages when requested */
			MODE_FORCE = 0x0, 
			/** Write messages when possible */
			MODE_DEFER = 0x1 };

		/**
		 * Logger levels.
		 * This levels are used as runtime filtering.
		 */
		enum LoggerLevel { 
			/** Debug level */
			LEVEL_DEBUG = 0x0, 
			/** Informational level */
			LEVEL_INFO = 0x1, 
			/** Notice level */
			LEVEL_NOTICE = 0x2, 
			/** Warning level */
			LEVEL_WARN = 0x3, 
			/** Error level */
			LEVEL_ERROR = 0x4, 
			/** Critical messages level */
			LEVEL_CRIT = 0x5, 
			/** Alert level */
			LEVEL_ALERT = 0x6, 
			/** Emergency messages level */
			LEVEL_EMERG = 0x7, 
			/** No level (line is logged) */
			LEVEL_NOLEVEL = 0xff };

		/**
		 * Constructor.
		 */
		Logger(void);

		/**
		 * Destructor.
		 */
		virtual ~Logger(void);

		/**
		 * Initialize logger.
		 * Each logger as its own name (which is NOT a filename). Logger name
		 * is used to have multiple loggers registred into an application, so
		 * that application modules can switch among loggers.
		 * @return SUCCESS or FAILURE if out of memory.
		 */
		int32 init(char* name);

		/**
		 * Get logger name.
		 */
		char* getName(void)
		{
			return m_dbName.toString();
		}

		/**
		 * Open log stream.
		 * @param stream_url an implementation dependent url. 
		 */
		virtual int32 open(char* url) = 0;

		/**
		 * Get log stream url.
		 */
		virtual char* getURL(void) = 0;

		/**
		 * Flush log buffer.
		 * Invoke this method to flush all buffered messages.
		 */
		virtual void flush(void) = 0;

		/**
		 * Close log stream
		 */
		virtual void close(void) = 0;

		/**
		 * Log a message.
		 * @param level one of the LEVEL constants.
		 * @param fmt format string like printf.
		 */
		virtual void log(uint8 level, const char* fmt, ...) = 0;

		/**
		 * Log a message.
		 * This method uses C variable arguments capability.
		 */
		virtual void log_va(uint8 level, const char* fmt, va_list ap) = 0;

		/**
		 * Log a message and exit from application.
		 */
		virtual void fail(const char* fmt, ...) = 0;

		/**
		 * Enable/disable logger.
		 */
		void setEnabled(bool flag);

		/**
		 * Check if logger is enabled.
		 */
		bool isEnabled(void);

		/**
		 * Set logger level.
		 */
		void setLevel(uint8 lvl);

		/**
		 * Get logger level.
		 */
		uint8 getLevel(void);

		/**
		 * Set mode.
		 */
		void setMode(uint8 mode);

		/**
		 * Get mode.
		 */
		uint8 getMode(void);
	private:
		/* copy constructor not available */
		Logger(Logger&)
		{
		}

		/* logger name */
		DataBuf m_dbName;

		/* log level */
		uint8 m_btLevel;

		/* logger mode */
		uint8 m_btMode;

		/* flag: logger enabled/disabled */
		bool m_bEnabled;
	};
}

#endif
