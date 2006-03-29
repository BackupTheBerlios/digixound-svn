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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "linux_file.hpp"

namespace uStreamLib {
	Impl_File::Impl_File(void)
		: _fd(-1)
	{
                // Nothing to do.
	}

	Impl_File::~Impl_File(void)
	{
		closeFile();
	}

	int32 Impl_File::openFile(char* pathname, int32 mode)
	{
		int32 sys_mode = 0;

		// close previous open file
		closeFile();

		// check mode
		if (mode == FileInputStream::FILE_MODE_READ)
			sys_mode = O_RDONLY;
		if (mode == FileInputStream::FILE_MODE_WRITE)
			sys_mode = O_WRONLY | O_CREAT;
		if (mode == FileInputStream::FILE_MODE_RW)
			sys_mode = O_RDWR | O_CREAT;

		int32 aux = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

		UOSUTIL_DOUT(("IMPL(openFile): opening %s\n", pathname));
		UOSUTIL_DOUT(("IMPL(openFile): mode    %d\n", mode));

		// open file
		_fd = ::open(pathname, sys_mode, aux);
		if (_fd < 0)
			return FAILURE;

		// ok
		return SUCCESS;
	}

	int32 Impl_File::closeFile(void)
	{
		if (_fd >= 0)
			::close(_fd);

		_fd = -1;
		return SUCCESS;
	}

	int32 Impl_File::readFile(void* data, size_t size)
	{
		if (_fd < 0)
			return FAILURE;
		return ::read(_fd, data, size);
	}

	int32 Impl_File::writeFile(void* data, size_t size)
	{
		if (_fd < 0)
			return FAILURE;
		return ::write(_fd, data, size);
	}

	int32 Impl_File::seek(int32 offset, int32 origin)
	{
		switch (origin) {
		case FileInputStream::SEEK_ORIGIN_SET:
			return ::lseek(_fd, offset, SEEK_SET);
		case FileInputStream::SEEK_ORIGIN_CUR:
			return ::lseek(_fd, offset, SEEK_CUR);
		case FileInputStream::SEEK_ORIGIN_END:
			return ::lseek(_fd, offset, SEEK_END);
		}

		return FAILURE;
	}

	int32 Impl_File::skip(int32 bytes)
	{
		int32 ret = ::lseek(_fd, bytes, SEEK_SET);
		if (ret < 0)
			return FAILURE;

		return ret;
	}
}

