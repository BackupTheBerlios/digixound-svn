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

#include "logger.hpp"
//#include "system.hpp"

namespace uStreamLib {
	Logger::Logger(void)
		: m_btLevel(Logger::LEVEL_NOLEVEL), m_btMode(Logger::MODE_FORCE),
		m_bEnabled(false)
	{
		Object::setClassID(UOSUTIL_RTTI_LOGGER);
	}

	Logger::~Logger(void)
	{
		// nothing to do
	}

	int32 Logger::init(char* name)
	{
		int32 ret = 0;

		// initialize parent
		ret = Mutex::init();
		if (ret == FAILURE)
			return FAILURE;

		// initialize buffers
		ret = m_dbName.init(name);
		if (ret == FAILURE)
			return FAILURE;

		// ok
		return SUCCESS;
	}

	void Logger::setEnabled(bool value)
	{
		MutexLocker ml(this);
		m_bEnabled = value;
	}

	bool Logger::isEnabled(void)
	{
		MutexLocker ml(this);
		return m_bEnabled;
	}

	void Logger::setLevel(uint8 value)
	{
		MutexLocker ml(this);
		m_btLevel = value;
	}

	uint8 Logger::getLevel(void)
	{
		MutexLocker ml(this);
		return m_btLevel;
	}

	void Logger::setMode(uint8 value)
	{
		MutexLocker ml(this);
		m_btMode = value;
	}

	uint8 Logger::getMode(void)
	{
		MutexLocker ml(this);
		return m_btMode;
	}
}
