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

#ifndef BLOCK_HANDLER_HPP
#define BLOCK_HANDLER_HPP

#include "callback.hpp"
#include "constants.hpp"

namespace uStreamLib {
	/*
	 * Forward declarations.
	 */
	class BlockManager;
	class Block;

	/**
	 * Block action and event callbacks.
	 * An action or event callback is a class that implements the method
	 * perform() and is used to handle specific block events and actions.
	 */
	class US_EXPORT BlockHandler : public CallBack {
	public:
		friend class Block;

		/**
		 * Return values for method perform().
		 */
		enum HandlerReturnValue { /** the handler did its job correctly */
		HSUCCESS = 0, /** the handler failed to make its job */
		HFAILURE = 1, /** the handler encountered a critical failure */
		HCRITICAL = 2 };

		/**
		 * Contructor.
		 * @param b the block this handler behaves to.
		 */
		BlockHandler(Block* b, char* name);

		/**
		 * Destructor.
		 */
		virtual ~BlockHandler(void);

		/**
		 * Get the associated block.
		 * @return pointer to the associated block.
		 */
		Block* getBlock(void)
		{
			return _block;
		}

		/**
		 * Get event or action code.
		 * @return code for action or event.
		 */
		uint32 getCode(void)
		{
			return _code;
		}
	protected:
		/**
		 * Handler entry point.
		 * When creating an event handler, this method must be overridden.
		 * This method must return HSUCCESS for signaling a success
		 * to the block; HFAILURE to signal that the handler failed
		 * and HCRITICAL to signal a critical failure.
		 */
		virtual int32 perform(void)
		{
			return SUCCESS;
		}
	private:
		/* copy constructor not available */
		BlockHandler(BlockHandler&)
		{
		}

		/* the block this handler belongs to */
		Block* _block;

		/* the event or action code this callback wish to handle */
		uint32 _code;
	};
}

#endif
