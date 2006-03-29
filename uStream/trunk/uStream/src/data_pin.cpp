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

#include "block.hpp"
#include "block_manager.hpp"

namespace uStreamLib {
	DataPin::DataPin(void)
	{
		ConfigTable::setClassID(UOSUTIL_RTTI_DATA_PIN);
	}

	DataPin::~DataPin(void)
	{
		MutexLocker ml(this);
	}

	int32 DataPin::init(Block* b, // block this pin belongs to
	char* name, // descriptive name
	uint8 dir, // pin direction
	uint32 bufsz, // preferred buffer size
	uint32 bufcount, // preferred count of buffers
	int32 queuesz     // queue size
	)
	{
		int32 ret = 0;

		// initialize parent
		ret = Pin::init(b, name, dir, bufsz, bufcount);
		if (ret == FAILURE)
			return FAILURE;

		// initialize queue 
		ret = _iq.init(getAbsoluteName(), sizeof(dmessage), queuesz);
		if (ret == FAILURE)
			return FAILURE;

		// setup data type
		setDataType(DT_UNDEF);

		// ok
		return SUCCESS;
	}

	int32 DataPin::sendMessage(dmessage* m, int32)
	{
		Enumeration* en = NULL;
		int32 ret = 0;

		if (getStatus() == Pin::UNCONNECTED) {
			puts("unconnected"); return FAILURE;
		}

		en = getPeers();
		while (en->hasMoreElements()) {
			DataPin* p = (DataPin*) en->nextElement();

			ret = p->_iq.put((char *) m, sizeof(dmessage));
			if (ret == FAILURE)
				return FAILURE;
		}

		return SUCCESS;
	}

	int32 DataPin::trySendMessage(dmessage* m, int32)
	{
		Enumeration* en = NULL;
		int32 ret = 0, ok = FAILURE;

		if (getStatus() == Pin::UNCONNECTED) {
			puts("unconnected"); return FAILURE;
		}

		en = getPeers();
		while (en->hasMoreElements()) {
			DataPin* p = (DataPin*) en->nextElement();

			ret = p->_iq.tryPut((char *) m, sizeof(dmessage));
			if (ret == SUCCESS)
				ok = SUCCESS;
		}

		return ok;
	}

	int32 DataPin::recvMessage(dmessage* m)
	{
		if (getStatus() == Pin::UNCONNECTED) {
			puts("unconnected"); return FAILURE;
		}

		return _iq.get((char *) m, sizeof(dmessage));
	}

	int32 DataPin::tryRecvMessage(dmessage* m)
	{
		if (getStatus() == Pin::UNCONNECTED) {
			puts("unconnected"); return FAILURE;
		}

		return _iq.tryGet((char *) m, sizeof(dmessage));
	}

	int32 DataPin::sendBuffer(DataBuf* buf, int32, avt_metadata* md,
		datainfo* di)
	{
		Enumeration* en = NULL;
		DataBuf* out = NULL;
		dmessage m;

		Logger* l = getBlock()->getBlockManager()->getLogger();

		l->log(Logger::LEVEL_DEBUG, "%s: sendBuffer(): [BID=%u,C=%u,SZ=%u]",
			getAbsoluteName(), buf->getBID(), buf->getCount(), buf->getSize());

		if (getStatus() == UNCONNECTED) {
			// log critical situation
			l->log(Logger::LEVEL_ERROR, "%s: SendBuffer: Not Connected",
				getBlock()->getName());

			// fail
			return FAILURE;
		}

		if (buf && !buf->getCount()) {
			// log critical situation
			l->log(Logger::LEVEL_CRIT,
				"%s: SendBuffer: EMPTY BUFFER (BID=%x,A=%x,C=%u,S=%u)",
				getBlock()->getName(), buf->getBID(), buf->getAddr(),
				buf->getCount(), buf->getSize());

			// fail
			return FAILURE;
		}

		// lock peers table
		lockTable(PEERS_TABLE);

		en = getPeers();
		while (en->hasMoreElements()) {
			DataPin* pin = (DataPin*) en->nextElement();

			/* ----- */

			/*
			   * Here, we need to lock current pin so that no
			   * modifications can be done to its internal properties.
			   */
			MutexLocker ml(pin);

			/* ----- */

			out = pin->getFreeBuffer();
			if (out) {
				out->xcopy(buf);

				m.bid = out->getBID();
				m.from = getBlock();
				m.from_pin = this;

				if (md)
					memcpy(&m.info, md, sizeof(avt_metadata));
				if (di)
					memcpy(&m.di, di, sizeof(datainfo));

				/*
					 * Do timestamping if SOURCE.
					 */
				if (getBlock()->getType() == Block::TYPE_SOURCE) {
					getBlock()->getBlockManager()->getClockTime(&m.di.td);
				}

				/*
					 * I cannot use sendMessage here because this method
					 * creates an enumeration of peers, invalidating the
					 * enum on which this cycle is based.
					 */
				pin->_iq.put((char *) &m, sizeof(dmessage));
			}
		}

		// unlock peers table
		unlockTable(PEERS_TABLE);

		// ok 
		return SUCCESS;
	}

	int32 DataPin::trySendBuffer(DataBuf* buf, int32, avt_metadata* md,
		datainfo* di)
	{
		Enumeration* en = NULL;
		DataBuf* out = NULL;

		dmessage m;

		int32 ret = 0, ok = FAILURE;
		Logger* l = getBlock()->getBlockManager()->getLogger();

		if (getStatus() == UNCONNECTED) {
			// log critical situation
			l->log(Logger::LEVEL_ERROR, "%s: TrySendBuffer: Not Connected",
				getBlock()->getName());

			// fail
			return FAILURE;
		}

		if (buf && !buf->getCount()) {
			// log critical situation
			l->log(Logger::LEVEL_CRIT,
				"%s: TrySendBuffer: EMPTY BUFFER (BID=%x,A=%x,C=%u,S=%u)",
				getBlock()->getName(), buf->getBID(), buf->getAddr(),
				buf->getCount(), buf->getSize());

			// fail
			return FAILURE;
		}

		// lock peers table
		lockTable(PEERS_TABLE);

		en = getPeers();
		while (en->hasMoreElements()) {
			DataPin* pin = (DataPin*) en->nextElement();

			/* ----- */

			/*
			 * Here, we need to lock current pin so that no
			 * modifications can be done to its internal properties.
			 */
			MutexLocker ml(pin);

			/* ----- */

			out = pin->tryGetFreeBuffer();
			if (out) {
				out->xcopy(buf);

				m.bid = out->getBID();
				m.from = getBlock();
				m.from_pin = this;

				if (md)
					memcpy(&m.info, md, sizeof(avt_metadata));
				if (di)
					memcpy(&m.di, di, sizeof(datainfo));

				/*
				 * Do timestamping if SOURCE.
				 */
				if (getBlock()->getType() == Block::TYPE_SOURCE) {
					getBlock()->getBlockManager()->getClockTime(&m.di.td);
				}

				/*
				 * I cannot use sendMessage here because this method
				 * creates an enumeration of peers, invalidating the
				 * enum on which this cycle is based.
				 */
				ret = pin->_iq.tryPut((char *) &m, sizeof(dmessage));
				if (ret == FAILURE) {
					l->log(Logger::LEVEL_EMERG,
						"%s: dmessage queue is full for pin %s",
						getAbsoluteName(), pin->getAbsoluteName());

					pin->freeInputBuffer(out->getBID());
				} else {
					ok = SUCCESS;
				}
			} else {
				l->log(Logger::LEVEL_EMERG,
					"%s: TrySendMessage(%s): no buffers in buffer pool",
					getAbsoluteName(), pin->getAbsoluteName());
			}
		}

		// unlock peers table
		unlockTable(PEERS_TABLE);

		// ok
		return ok;
	}
}
