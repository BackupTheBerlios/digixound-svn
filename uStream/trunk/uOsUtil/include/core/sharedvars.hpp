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

#ifndef SHAREDVARS_HPP
#define SHAREDVARS_HPP

#include "databuf.hpp"
#include "mutex.hpp"

namespace uStreamLib {
	/**
	 * This class is a shared variable among threads. Shared variables
	 * are normal variables such as integers, doubles and strings, which
	 * can be accessed in read/write operations in a thread safe way.
	 * This is a shared integer.
	 */
	class US_API_EXPORT SharedInt : public Mutex {
	public:
		/**
		 * Constructor.
		 */
		SharedInt(void);

		/**
		 * Destructor.
		 */
		virtual ~SharedInt(void);

		/**
		 * Build a shared integer.
		 * @param value optional initializer.
		 */
		int32 init(int32 value = 0);

		SharedInt& operator=(int32 value)
		{
			MutexLocker ml(this); _value = value; return *this;
		}

		void operator++(void)
		{
			MutexLocker ml(this); _value++;
		}

		void operator--(void)
		{
			MutexLocker ml(this); _value--;
		}

		void operator+=(int32 add)
		{
			MutexLocker ml(this); _value += add;
		}

		void operator-=(int32 sub)
		{
			MutexLocker ml(this); _value -= sub;
		}

		int32 get(void)
		{
			MutexLocker ml(this); return _value;
		}
	private:
		/* the signed integer value */
		int32 _value;
	};

	/**
	 * This class is a shared variable among threads. Shared variables
	 * are normal variables such as integers, doubles and strings, which
	 * can be accessed in read/write operations in a thread safe way.
	 * This is a shared unsigned integer.
	 */
	class US_API_EXPORT SharedUint : public Mutex {
	public:
		/**
		 * Constructor.
		 */
		SharedUint(void);

		/**
		 * Destructor.
		 */
		virtual ~SharedUint(void);

		/**
		 * Build a shared unsigned integer.
		 * @param value optional initializer.
		 */
		int32 init(uint32 value = 0);

		SharedUint& operator=(uint32 value)
		{
			MutexLocker ml(this); _value = value; return *this;
		}

		void operator++(void)
		{
			MutexLocker ml(this); _value++;
		}

		void operator--(void)
		{
			MutexLocker ml(this); _value--;
		}

		void operator+=(int32 add)
		{
			MutexLocker ml(this); _value += add;
		}

		void operator-=(int32 sub)
		{
			MutexLocker ml(this); _value -= sub;
		}

		uint32 get(void)
		{
			MutexLocker ml(this); return _value;
		}
	private:
		/* the unsigned integer value */
		uint32 _value;
	};

	/**
	 * This class is a shared variable among threads. Shared variables
	 * are normal variables such as integers, doubles and strings, which
	 * can be accessed in read/write operations in a thread safe way.
	 * This is a shared double.
	 */
	class US_API_EXPORT SharedDouble : public Mutex {
	public:
		/**
		 * Constructor.
		 */
		SharedDouble(void);

		/**
		 * Destructor.
		 */
		virtual ~SharedDouble(void);

		/**
		 * Build a shared double.
		 * @param value optional initializer.
		 */
		int32 init(double value = 0.0);

		SharedDouble& operator=(double value)
		{
			MutexLocker ml(this); _value = value; return *this;
		}

		double get(void)
		{
			MutexLocker ml(this); return _value;
		}
	private:
		/* the double value */
		double _value;
	};

	/**
	 * This class is a shared variable among threads. Shared variables
	 * are normal variables such as integers, doubles and strings, which
	 * can be accessed in read/write operations in a thread safe way.
	 * This is a shared double.
	 */
	class US_API_EXPORT SharedString : public Mutex {
	public:
		/**
		 * Constructor.
		 */
		SharedString(void);

		/**
		 * Destructor.
		 */
		virtual ~SharedString(void);

		/**
		 * Build a shared string.
		 * @param value optional initializer.
		 */
		int32 init(char* value = NULL);

		/**
		 * Build a shared string.
		 * @param size initial size of the string.
		 */
		int32 init(uint32 size);

		SharedString& operator=(char* value)
		{
			MutexLocker ml(this); _value.xcopy(value); return *this;
		}

		SharedString& operator=(SharedString& value)
		{
			MutexLocker ml(this); _value.xcopy(value.get()); return *this;
		}

		char* get(void)
		{
			MutexLocker ml(this); return _value.toString();
		}
	private:
		/* copy constructor not available */
		SharedString(SharedString&)
		{
		}

		/* the value */
		DataBuf _value;
	};
}

#endif
