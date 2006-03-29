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

#ifndef FILEIO_HPP
#define FILEIO_HPP

#include "object.hpp"
#include "databuf.hpp"

namespace uStreamLib {

        /*
         * Forward declaration for implementation.
         */
        class Impl_File;

	class US_API_EXPORT FileInputStream : public Object {
	public:
		/**
		 * File modes.
		 */
		enum FISOpenMode { FILE_MODE_READ = 0, FILE_MODE_WRITE = 1,
		FILE_MODE_RW = 2, FILE_MODE_TRUNCATE = 3 };

		/*
		 * Origin constants for seek().
		 */
		enum { SEEK_ORIGIN_SET = 0, SEEK_ORIGIN_CUR = 1, SEEK_ORIGIN_END = 2 };

		/**
		 * Constructor.
		 */
		FileInputStream(void);

		/**
		 * Destructor.
		 * It closes the file and releases all used resources.
		 */
		virtual ~FileInputStream(void);

		/**
		 * Build a FileInputStream.
		 * @param filename the path of the file to open for reading.
		 */
		int32 init(char* filename);

		/**
		 * Close the input stream, releasing all used resources.
		 */
		void close(void);

		/**
		 * Mark the current position in this input stream. A subsequent call to the
		 * reset method repositions this stream at the last marked position so that
		 * subsequent reads re-read the same bytes.
		 * The readlimit arguments tells this input stream to allow that many bytes
		 * to be read before the mark position gets invalidated.
		 * The general contract of mark is that, if the method markSupported
		 * returns true, the stream somehow remembers all the bytes read after the
		 * call to mark and stands ready to supply those same bytes again if and
		 * whenever the method reset is called. However, the stream is not required
		 * to remember any data at all if more than readlimit bytes are read from
		 * the stream before reset is called.
		 */
		void mark(int32 readlimit);

		/**
		 * Test if mark() and reset() methods are supported by this stream.
		 * @return true or false.
		 */
		bool markSupported(void);

		/**
		 * Read one byte from the input stream.
		 * @param byte a pointer to a 1 character buffer for storing the read byte.
		 * @return the number of bytes read (1 of course) or FAILURE if an error
		 * occurs.
		 */
		int32 read(char* byte);

		/**
		 * Read up to size bytes and store the in the provided buffer.
		 * @param buf a buffer to store bytes into.
		 * @param size number of bytes to read.
		 * @return the number of bytes read, 0 in case of EOF or FAILURE if an error
		 * occurs.
		 */
		int32 read(char* buf, uint32 size);

		/**
		 * Read up to getSize() bytes from the stream and store them in the provided
		 * databuf.
		 * @param buf the databuf to store read bytes into.
		 * @return the number of bytes read, 0 in case of EOF or FAILURE if an error
		 * occurs.
		 */
		int32 read(DataBuf* buf);

		/**
		 * Read up to size bytes from the stream and store them in the provided
		 * databuf.
		 * @param buf the databuf to store read bytes into.
		 * @param offset the index in buf in which to start storage of bytes.
		 * @param size number of bytes to read.
		 * @return the number of bytes read, 0 in case of EOF or FAILURE if an error
		 * occurs.
		 */
		int32 read(DataBuf* buf, uint32 offset, uint32 size);

		/**
		 * Reposition the stream to the position at the time the mark() method
		 * was last invoked.
		 * @return the position in the stream or FAILURE if reset() cannot be
		 * performed.
		 */
		int32 reset(void);

		/**
		 * Skip over and discard n bytes of data in the stream.
		 * @param n number of bytes to discard.
		 * @return number of discarded bytes or FAILURE if skipping cannot be
		 * performed.
		 */
		int32 skip(int32 n);

		/**
		 * Method to seek in the stream.
		 * @param offset the offset position in bytes.
		 * @param from the origin from where to seek.
		 * @return FAILURE in case of errors or the current offset in bytes from
		 * the start.
		 */
		int32 seek(int32 offset, int32 from);

		/**
		 * Get error string.
		 */
		char* getErrorString(void)
		{
			return _error.toString();
		}

	private:
		/* copy constructor not available */
		FileInputStream(FileInputStream&):Object(UOSUTIL_RTTI_FILE_INPUT_STREAM)
		{
		}

		/* error string buffer */
		DataBuf _error;

                /* specific implementation object */
		Impl_File* _impl;
	};

	class US_API_EXPORT FileOutputStream : public Object {
	public:
		/**
		 * File modes.
		 */
		enum FOSOpenMode { FILE_MODE_READ = 0, FILE_MODE_WRITE = 1,
		FILE_MODE_RW = 2, FILE_MODE_TRUNCATE = 3 };

		/**
		 * Constructor.
		 */
		FileOutputStream(void);

		/**
		 * Destructor.
		 * It closes the file and releases all used resources.
		 */
		virtual ~FileOutputStream(void);

		/**
		 * Build a FileOutputStream.
		 * @param filename the path of the file to open for writing.
		 */
		int32 init(char* filename, int32 mode = FILE_MODE_RW);

		/**
		 * Close the stream, releasing all used resources.
		 */
		void close(void);

		/**
		 * Write one byte into the output stream.
		 * @param byte the character to write.
		 * @return the number of bytes written (1 of course) or FAILURE if an error
		 * occurs.
		 */
		int32 write(char byte);

		/**
		 * Write up to size bytes getting them from the provided buffer.
		 * @param buf a buffer containing the bytes to write.
		 * @param size number of bytes to write.
		 * @return the number of bytes written or FAILURE if an error
		 * occurs.
		 */
		int32 write(char* buf, uint32 size);

		/**
		 * Write up to getCount() bytes to the stream.
		 * @param buf the databuf from which to get bytes.
		 * @return the number of bytes written or FAILURE if an error
		 * occurs.
		 */
		int32 write(DataBuf* buf);

		/**
		 * Write up to size bytes in the stream.
		 * @param buf the databuf to get bytes from.
		 * @param offset the index in buf in which to start to get bytes.
		 * @param size number of bytes to write.
		 * @return the number of bytes written or FAILURE if an error
		 * occurs.
		 */
		int32 write(DataBuf* buf, uint32 offset, uint32 size);

		/**
		 * Method to seek in the stream.
		 * @param offset the offset position in bytes.
		 * @param from the origin from where to seek.
		 * @return FAILURE in case of errors or the current offset in bytes from
		 * the start.
		 */
		int32 seek(int32 offset, int32 from);

		/**
		 * Get error string.
		 */
		char* getErrorString(void)
		{
			return _error.toString();
		}
	private:
		/* copy constructor not available */
		FileOutputStream(FileOutputStream&):Object(UOSUTIL_RTTI_FILE_OUTPUT_STREAM)
		{
		}

		/* error string buffer */
		DataBuf _error;

                /* specific implementation object */
		Impl_File* _impl;
	};
}

#endif
