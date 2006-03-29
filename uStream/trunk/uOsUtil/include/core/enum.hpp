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

#ifndef ENUM_HPP
#define ENUM_HPP

#include "object.hpp"

namespace uStreamLib {
	/**
	 * Class used to enumerate items into containers. Hash and Vector
	 * produce Enumerations so that one can get all elements in a linear
	 * way.
	 */
	class US_API_EXPORT Enumeration : public Object {
	public:
		/**
		 * Constructor.
		 */
		Enumeration(void);

		/**
		 * Destructor.
		 */
		virtual ~Enumeration(void);

		/**
		 * When implementing a storage class such as Hash and Vector or similar,
		 * you can create internal enumerations and then return them after having
		 * filled them with values. Enumeration has a size that grows dynamically
		 * by adding new elements.
		 * @param enumeration size.
		 */
		int32 init(uint32 size);

		/**
		 * Call this method to check if enumeration has more elements. This
		 * method returns true if there are more elements to get. Get an element
		 * by using nextElement() method. You need to cast to your object for
		 * using that element.
		 * @return true or false.
		 */
		bool hasMoreElements(void)
		{
			return (m_uCurItem < m_uCount);
		}

		/**
		 * This method returns the next element in the enumeration if there is one
		 * available. Don't call this method if hasMoreElements() has returned false.
		 * @return a pointer to the next element. You need to cast.
		 */
		void* nextElement(void)
		{
			void* retval = m_peiCur->value;

			m_uCurItem += 1;
			m_peiCur += 1;

			return retval;
		}

		/**
		 * This method is similar to nextElement(). By invoking this method you can
		 * retrieve next element and its size, by passing the address of a uint32
		 * variable in which to store next element's size.
		 * @param item_size output parameter for storing size.
		 * @return the next element as opaque pointer.
		 */
		void* nextElement(uint32* item_size);

		/**
		 * Rewind the enumeration so that you can call hasMoreElements() and
		 * nextElement() again.
		 */
		void rewind(void)
		{
			m_peiCur = m_peiItems;
			m_uCurItem = 0;
		}

		/**
		 * Clear the enumeration. It becomes empty.
		 */
		void clear(void)
		{
			m_peiCur = m_peiItems;
			m_uCount = 0;
			m_uCurItem = 0;
		}

		/**
		 * Get count of elements in the enumeration.
		 * @return count of elements.
		 */
		uint32 getCount(void)
		{
			return m_uCount;
		}

		/**
		 * Get size of enumeration.
		 * @return size.
		 */
		uint32 getSize(void)
		{
			return m_uSize;
		}

		/**
		 * Add an element to the enumeration. You must specify the element
		 * to add (a pointer) and its size if any. This method make the
		 * enumeration grow accordingly with its previous size.
		 * @param item an opaque pointer to the element to add.
		 * @param size item's size if any.
		 */
		void addElement(void* item, uint32 size);
	private:
		/* enumeration item type */
		struct EnumItem {
			/* pointer to the real value */
			void* value;

			/* size of the item */
			uint32 size;
		};

		/* array of enumeration items */
		EnumItem* m_peiItems;

		/* total count of items */
		uint32 m_uCount;

		/* size of enumeration */
		uint32 m_uSize;

		/* current item index */
		uint32 m_uCurItem;

		/* current item pointer */
		EnumItem* m_peiCur;
	};
}

#endif
