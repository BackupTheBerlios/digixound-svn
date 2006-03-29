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

#ifndef LOGGER_STDOUT_HPP
#define LOGGER_STDOUT_HPP

#include "logger.hpp"

namespace uStreamLib
{
  class US_API_EXPORT LoggerStdOut: public Logger
  {
  public:
    /**
     * Constructor.
     */
    LoggerStdOut(void);

    /**
     * Destructor.
     */
    virtual ~LoggerStdOut(void);

    /**
     * Initialize standard output device logger.
     * This logger will log each line in the standard output device.
     * @param name logger name.
     */
    int32 init(char *name);

    /*
     * Methods from logger.
     */
    virtual int32 open(char *filename){}
    virtual void flush(void){}
    virtual void close(void){}
    virtual char *getURL(void){}
    virtual void fail(const char *fmt,...){}


    virtual void log(uint8 level,const char *fmt,...);
    virtual void log_va(uint8 level,const char *fmt,va_list ap);
  private:
 };
}

#endif

