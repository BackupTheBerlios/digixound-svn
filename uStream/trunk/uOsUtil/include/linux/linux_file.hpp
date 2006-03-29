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

#ifndef LINUX_FILE_HPP
#define LINUX_FILE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <utime.h>

#include "fileio.hpp"

namespace uStreamLib {
	class Impl_File {
	public:
		/* constructor */
		Impl_File(void);

		/* destructor */
		~Impl_File(void);


		/* open/close api */
		int32 openFile(char* pathname, int32 mode);
		int32 closeFile(void);

		/* read/write api */
		int32 readFile(void* data, size_t size);
		int32 writeFile(void* data, size_t size);

		/* movement api */
		int32 seek(int32 offset, int32 origin);
		int32 skip(int32 bytes);
	private:
		/* file descriptor */
		int32 _fd;
	};
}

#endif
