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

#ifndef SOURCE_HPP
#define SOURCE_HPP

#include "block.hpp"

namespace uStreamLib {
	/**
	 * This class is a Source.
	 * A source is a specialized Block which emits data
	 * as its main task.
	 */
	class US_EXPORT Source : public Block {
	public:
		/* friend classes */
		friend class BlockManager;


		/**
				 * Constructor.
				 */
		Source(void);

		/**
				 * Destructor.
				 */
		virtual ~Source(void);

		/**
				 * Redefined method to signal this is a Source.
				 * @return Block::TYPE_SOURCE.
				 */
		int32 getType(void)
		{
			return Block::TYPE_SOURCE;
		}

		/**
				 * Source thread entry point.
				 */
		void run(void);

	protected:
		/**
			 * Build a source. A source inherits from Block.
			 * @param bm a pointer to the block manager.
			 * @param name a descriptive name of this source.
			 * @param bufsz preferred control pin's buffer size.
			 * @param bufcount preferred control pin's buffers count.
			 * @param queuesz control pin's internal priority queue size.
			 */
		int32 init(BlockManager* bm, // pointer to block manager
				  char* name, // descriptive name
				uint32 bufsz = US_CP_BUFSZ, // control pin's preferred buffer size
			uint32 bufcount = US_CP_BUFCO, // control pin's preferred buffer count
				int32 queuesz = US_CP_QUEUESZ  // control pin's queue size
		);

	private:
		/*
			* Other constructors.
			*/
		Source(Source&)
		{
		}

		/* flag: the source started */
		bool _started;

		/* flag: termination request received */
		bool _quit;

		/* quit semaphore */
		Semaphore _quitSem;
	};
}

#endif
