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

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "enum.hpp"
#include "pvector_unsafe.hpp"

namespace uStreamLib {
	/**
	 * This class is a dynamic vector of elements.
	 * The vector can grow when more elements are added.
	 * Each element can have a default size.
	 */
	class US_API_EXPORT Vector : public Object {
	public:
		/**
		 * Constructor.
		 */
		Vector(void);

		/**
		 * Destructor.
		 */
		virtual ~Vector(void);

		/**
		 * Build a vector of elements.
		 * The vector can grow dynamically when new elements are added.
		 * @param isize initial item size.
		 * @param cap initial capacity (no. of items).
		 * @param incr number of new item slot to add when the vector grows.
		 */
		int32 init(uint32 isize, uint32 cap = 5, uint32 incr = 3);

		/**
		 * Check if this vector is sorted.
		 */
		bool isSorted(void);

		/**
		 * Sort NOW the elements in the vector.
		 * @param comp a Comparator object that will perform the comparisons.
		 */
		int32 sort(Comparator* comp);

		/**
		 * Search an element.
		 * @param data the key for this search.
		 * @param sz the key size.
		 * @return the value if found or NULL if not found.
		 */
		char* search(char* data, uint32 sz, Comparator* c = NULL);

		/**
		 * Search an element that IS A STRING.
		 * @param data the key for this search.
		 * @return the value if found or NULL if not found.
		 */
		char* search(char* data, Comparator* c = NULL)
		{
			return search(data, (uint32) strlen(data) + 1, c);
		}

		/**
		 * Operator[] to get a pointer to the i-th element.
		 * @return NULL if i is out of range.
		 */
		char* operator[](uint32 index);

		/**
		 * Get an enumeration of elements in the vector.
		 */
		Enumeration* elements(void);

		/**
		 * Add elements to the vector.
		 * The elements are contained into a formatted string which will
		 * be parsed using "separator" as the separator string.
		 * @param elements the container string of separated elements.
		 * @param separator the separator string to use for tokenization.
		 * @param trim remove trailing whitespace.
		 */
		int32 addElements(char* elements, char* separator, bool trim = true);

		/**
		 * Add the specified element to the vector.
		 * @param elem a pointer to the element to add.
		 * @param sz element size.
		 * @return SUCCESS or FAILURE if the vector cannot grow further.
		 */
		int32 addElement(char* elem, uint32 sz);

		/**
		 * Add the specified null terminated string to the vector.
		 * @param elem a pointer to a null terminated string.
		 * @return SUCCESS or FAILURE if the vector cannot grow further.
		 */
		int32 addElement(char* elem)
		{
			return addElement(elem, (uint32) strlen(elem) + 1);
		}

		/**
		 * Put specified element at index.
		 * @param index position in the vector to put the element into.
		 * @param elem a pointer to the element to add to the vector.
		 * @param sz size of the element.
		 * @return SUCCESS or FAILURE if index is out of range.
		 */
		int32 setElementAt(uint32 index, char* elem, uint32 sz);

		/**
		 * Put specified null terminated string at index.
		 * @param index position in the vector to put the element into.
		 * @param elem a pointer to a null terminated string.
		 * @return SUCCESS or FAILURE if index is out of range.
		 */
		int32 setElementAt(uint32 index, char* elem)
		{
			return setElementAt(index, elem, (uint32) strlen(elem) + 1);
		}

		/**
		 * Remove element at index.
		 */
		int32 removeElementAt(uint32 idx);

		/**
		 * Remove the first occurrence of elem.
		 * @param elem a pointer to the element to remove.
		 * @param sz size of the element.
		 * @param c a pointer to a comparator object.
		 * @return SUCCESS or FAILURE if element not found.
		 */
		int32 removeElement(char* elem, uint32 sz, Comparator* c = NULL);

		/**
		 * Remove the first occurrence of elem.
		 * @param elem a pointer to a null terminated string.
		 * @param c a pointer to a comparator object.
		 * @return SUCCESS or FAILURE if element not found.
		 */
		int32 removeElement(char* elem, Comparator* c = NULL)
		{
			return removeElement(elem, (uint32) strlen(elem) + 1, c);
		}

		/**
		 * Remove all elements.
		 * The vector will not change its size. Memory will be the same as
		 * before this call.
		 */
		void removeAll(void);

		/**
		 * Remove the last element only.
		 */
		void removeLast(void);

		/**
		 * Perform a linear search (scan) for the specified element.
		 * @param elem a pointer to the element so search for.
		 * @param sz size of the element.
		 * @param index out: the index of the found element.
		 * @return SUCCESS or FAILURE if element not found.
		 */
		int32 scan(char* elem, uint32 sz, uint32* index, Comparator* c = NULL);

		/**
		 * Perform a linear search (scan) for the specified element.
		 * @param elem a pointer to a null terminated string.
		 * @param index out: the index of the found element.
		 * @return SUCCESS or FAILURE if element not found.
		 */
		int32 scan(char* elem, uint32* index, Comparator* c = NULL)
		{
			return scan(elem, (uint32) strlen(elem) + 1, index, c);
		}

		/**
		 * Get comparator object.
		 */
		Comparator* getComparator(void)
		{
			return m_comp;
		}

		/**
		 * Set comparator object.
		 */
		void setComparator(Comparator* c)
		{
			m_comp = c;
		}

		/**
		 * Get elements count.
		 */
		uint32 getCount(void)
		{
			return m_uCount;
		}
		
	private:
		/* pointer vector (container of databuf) */
		PVectorUnsafe m_pvDB;

		/* work buffer for "add elements" method */
		DataBuf m_dbWork;

		/* sorting flag */
		bool m_bSorted;

		/* comparator for sorting elements */
		Comparator* m_comp;

		/* items count */
		uint32 m_uCount;

		/* elements enumeration */
		Enumeration m_enValues;

		/* method for sorting items */
		void _quickSort(int32 idx1, int32 idx2, Comparator* comp);
	};
}

#endif
