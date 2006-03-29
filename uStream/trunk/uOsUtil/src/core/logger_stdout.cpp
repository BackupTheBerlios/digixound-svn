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

#include <iostream>
#include "logger_stdout.hpp"

namespace uStreamLib
{
  LoggerStdOut::LoggerStdOut(void)
  {
    // Nothing to do.
  }

  LoggerStdOut::~LoggerStdOut(void)
  {
    // Nothing to do.
  }

  int32 LoggerStdOut::init(char *name)
  {
    int32 ret = 0;

    // Initialize parent.
    ret = Logger::init(name);
    if (ret == FAILURE) return FAILURE;

    // enable logger
    setEnabled(true);

    // ok
    return SUCCESS;
  }

  void LoggerStdOut::log(uint8 level,const char *fmt,...)
  {
    va_list ap;

    va_start(ap,fmt);
    log_va(level,fmt,ap);
    va_end(ap);
  }

  void LoggerStdOut::log_va(uint8 level,const char *fmt,va_list ap)
  {
    char tmp[256];

    if (isEnabled())
    {
      if (level >= getLevel())
      {
        vsnprintf(tmp, 256, fmt, ap);

        std::cout << tmp << "\n";
      }
    }
  }


}
