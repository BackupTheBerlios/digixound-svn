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

#include "fileio.hpp"

/*
 * Here, we choose the right implementation using
 * conditional compilation.
 */

#if defined(_WIN32) || defined(WIN32)
#include "win32_file.hpp"
#else
#include "linux_file.hpp"
#endif

/*
 * This class is a proxy for specific implementation.
 */

namespace uStreamLib {
	/*
	* Implementation of FileInputStream.
	*/

	FileInputStream::FileInputStream(void):Object(UOSUTIL_RTTI_FILE_INPUT_STREAM)
	{
                _impl = new Impl_File();
        }

	FileInputStream::~FileInputStream(void)
	{
		close();
                delete _impl;
	}

	int32 FileInputStream::init(char* filename)
	{
		int32 ret = 0;

		// initialize error string
		ret = _error.init("No error");
		if (ret == FAILURE)
			return FAILURE;

		// invoke implementation
		ret = _impl->openFile(filename, FILE_MODE_READ);
		if (ret == FAILURE)
			return FAILURE;

		// ok
		return SUCCESS;
	}

	void FileInputStream::close(void)
	{
		_impl->closeFile();
	}

	void FileInputStream::mark(int32)
	{
		UOSUTIL_DERR((stderr, "FileInputStream::mark() ** UNSUPPORTED **\n"));
	}

	bool FileInputStream::markSupported(void)
	{
		return false;
	}

	int32 FileInputStream::seek(int32 offset, int32 origin)
	{
		return _impl->seek(offset, origin);
	}

	int32 FileInputStream::read(char* byte)
	{
		return _impl->readFile(byte, 1);
	}

	int32 FileInputStream::read(char* buf, uint32 size)
	{
		return _impl->readFile(buf, size);
	}

	int32 FileInputStream::read(DataBuf* buf)
	{
		int32 ret = _impl->readFile(buf->getAddr(), buf->getSize());
		if (ret == FAILURE)
			return FAILURE;

		buf->setCount(ret);
		return ret;
	}

	int32 FileInputStream::read(DataBuf* buf, uint32 offset, uint32 size)
	{
		char* addr = (char*) buf->getAddr();
		uint32 toread = 0, readable = 0;
		int32 ret = 0;

		if (size > buf->getSize()) {
			_error.xcopy("size greater than buffer size"); return FAILURE;
		}
		if (offset >= size || offset >= buf->getSize()) {
			_error.xcopy("invalid offset"); return FAILURE;
		}

		readable = buf->getSize() - offset;
		if (size >= readable)
			toread = readable;
		else
			toread = size;

		ret = _impl->readFile(&addr[offset], toread);
		if (ret == FAILURE)
			return FAILURE;

		buf->setCount(offset + ret);
		return ret;
	}

	int32 FileInputStream::reset(void)
	{
		UOSUTIL_DERR((stderr, "FileInputStream::reset() ** UNSUPPORTED **\n"));
		return 0;
	}

	int32 FileInputStream::skip(int32 n)
	{
		return _impl->skip(n);
	}

	/*
	* Implementation of FileOutputStream.
	*/

	FileOutputStream::FileOutputStream(void):Object(UOSUTIL_RTTI_FILE_OUTPUT_STREAM)
	{
                _impl = new Impl_File();
        }

	FileOutputStream::~FileOutputStream(void)
	{
		close();
                delete _impl;
	}

	int32 FileOutputStream::init(char* filename, int32 mode)
	{
		int32 ret = 0;

		// initialize error string
		ret = _error.init("No error");
		if (ret == FAILURE)
			return FAILURE;

		// invoke implementation
		ret = _impl->openFile(filename, mode);
		if (ret == FAILURE)
			return FAILURE;

		// ok
		return SUCCESS;
	}

	void FileOutputStream::close(void)
	{
		_impl->closeFile();
	}

	int32 FileOutputStream::seek(int32 offset, int32 origin)
	{
		return _impl->seek(offset, origin);
	}

	int32 FileOutputStream::write(char byte)
	{
		return _impl->writeFile(&byte, 1);
	}

	int32 FileOutputStream::write(char* buf, uint32 size)
	{
		return _impl->writeFile(buf, size);
	}

	int32 FileOutputStream::write(DataBuf* buf)
	{
		return _impl->writeFile(buf->getAddr(), buf->getCount());
	}

	int32 FileOutputStream::write(DataBuf* buf, uint32 offset, uint32 size)
	{
		char* addr = (char*) buf->getAddr();
		uint32 towrite = 0, writeable = 0;

		if (size > buf->getCount()) {
			_error.xcopy("size greater than buffer size"); return FAILURE;
		}
		if (offset >= size || offset >= buf->getCount()) {
			_error.xcopy("invalid offset"); return FAILURE;
		}

		writeable = buf->getCount() - offset;
		if (size >= writeable)
			towrite = writeable;
		else
			towrite = size;

		return _impl->writeFile(&addr[offset], towrite);
	}
}
