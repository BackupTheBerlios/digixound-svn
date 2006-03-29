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

#ifndef PSTACK_HPP
#define PSTACK_HPP

#include "mutex.hpp"

namespace uStreamLib {
	/**
	 * A stack of pointers. Thread safe.
	 *
	 * This structure allows to manage a stack of pointers.
	 * This means that you can push and pop objects, but
	 * their allocation must be done by you. No malloc, copy
	 * or free is made by the functions in this module.
	 *
	 * The stack is of fixed size and cannot be expanded. So
	 * be careful and detect push() and pop() errors.
	 */
	class US_API_EXPORT PStack : public Object {
	public:
		/**
		 * Constructor.
		 */
		PStack(void);

		/**
		 * Destructor.
		 */
		virtual ~PStack(void);

		/**
		 * Initialize the stack.
		 * @param stack size.
		 */
		int32 init(uint32 size);

		/**
		 * Get stack size.
		 */
		int32 getSize(void)
		{
			return m_iSize;
		}

		/**
		 * Get stack pointer.
		 * This method is not thread safe.
		 */
		int32 getStackPointer(void);

		/**
		 * Get element at the top of the stack.
		 * The element is not removed from the stack.
		 * This method is not thread safe.
		 * @return a void pointer. A cast is needed.
		 */
		void* getTop(void);

		/**
		 * Push an item into the stack.
		 * @return SUCCESS or FAILURE if the stack is full.
		 */
		int32 push(void* item);

		/**
		 * Pop an item from the stack.
		 * @return a pointer to the element or NULL if
		 * the stack is empty.
		 */
		void* pop(void);

	private:
		/* array of memory pointers (void *) */
		void** m_ptrVoidItems;

		/* current stack pointer */
		int32 m_iStackPointer;

		/* size of the stack */
		int32 m_iSize;

		/* global stack mutex */
		Mutex m_mutex;
	};
}

#endif
