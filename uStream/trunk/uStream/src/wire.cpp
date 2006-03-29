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

#include "wire.hpp"
#include "pin.hpp"

namespace uStreamLib {
	Wire::Wire(void)
		: Object(UOSUTIL_RTTI_WIRE), _type(0), _bp1(NULL), _bp2(NULL),
		_p1(NULL), _p2(NULL), _multipoint_on_bp1(false),
		_multipoint_on_bp2(false)
	{
		// nothing to do
	}

	Wire::~Wire(void)
	{
		int32 ret = 0;

		// DEBUG
		UOSUTIL_DOUT(("~Wire(): entered"));

		if (_p1 && _p2) {
			MutexLocker ml1(_p1);
			MutexLocker ml2(_p2);

			ret = _p1->disconnect(_p2->getBlock(), _p2);
			if (ret == FAILURE) {
				// DEBUG
				UOSUTIL_DOUT(("~Wire(): disconnect failure on %s\n",
					_p1->getAbsoluteName()));
			}

			ret = _p2->disconnect(_p1->getBlock(), _p1);
			if (ret == FAILURE) {
				// DEBUG
				UOSUTIL_DOUT(("~Wire(): disconnect failure on %s\n",
					_p2->getAbsoluteName()));
			}

			/*
			 * Pin2 is an input pin for sure, so the buffer pool is shared among
			 * wires. If there are no more peers connected to pin2, it means
			 * that no more wires share buffer pool and this is the last wire.
			 * So delete the bp2 buffer pool.
			 */

			if (!_p2->getPeersCount()) {
				delete _bp2; _p2->_ibp = NULL; _p2->_bpSet = false;
			}

			/*
			 * In case of bidirectional wire, Pin1 behaves like Pin2. So
			 * check for type and do the same us below.
			 */
			if (_type == BIDIRECTIONAL) {
				// DEBUG
				UOSUTIL_DOUT(("~Wire(): doing cleanup for bidirectional wire\n"));
				UOSUTIL_DOUT(("~Wire(): p1 = %s, p2 = %s\n",
					_p1->getAbsoluteName(), _p2->getAbsoluteName()));

				if (!_p1->getPeersCount()) {
					delete _bp1; _p1->_ibp = NULL; _p1->_bpSet = false;
				}
			}
		}

		// DEBUG
		UOSUTIL_DOUT(("~Wire(): ok"));
	}

	int32 Wire::init(int32 type)
	{
		int32 ret = 0;

		// initialize buffers
		ret = _error_string.init("");
		if (ret == FAILURE)
			return FAILURE;

		// setup type
		_type = type;

		// ok
		return SUCCESS;
	}

	int32 Wire::connect(Block* b1, Pin* p1, Block* b2, Pin* p2)
	{
		Pin::SubType* st1 = NULL, * st2 = NULL;
		Block* tmp_b1 = NULL, * tmp_b2 = NULL;

		int32 ret = 0;
		char tmp[4096];

		// check if both pins are already connected
		if (p1->isConnectedTo(p2)) {
			_error_string = "pins already connected to each other";
			return FAILURE;
		}

		// check pins directions
		if (p1->getDirection() == p2->getDirection()) {
			if (p1->getDirection() != Pin::DIR_IO) {
				_error_string = "both pins have same direction";
				return FAILURE;
			}
		}

		// check data flavour
		if ((p1->getDataType() != DT_UNDEF && p1->getDataType() != DT_BYTES) &&
			(p2->getDataType() != DT_UNDEF && p2->getDataType() != DT_BYTES)) {
			if (p1->getDataType() != p2->getDataType()) {
				snprintf(tmp, sizeof(tmp),
					"incompatible data flavours: %s != %s",
					Pin::getDataTypeString(p1->getDataType()),
					Pin::getDataTypeString(p2->getDataType()));

				_error_string = tmp;
				return FAILURE;
			}
		}

		// check sub types
		st1 = p1->getSubType();
		st2 = p2->getSubType();

		if (st1->vf != VF_UNDEF && st2->vf != VF_UNDEF) {
			if (st1->vf != st2->vf) {
				switch (p1->getDataType()) {
				case DT_VIDEO:
					snprintf(tmp, sizeof(tmp),
						"incompatible sub types: %s != %s",
						VideoFormats::getVideoFormatString(st1->vf),
						VideoFormats::getVideoFormatString(st2->vf));
					break;
				case DT_AUDIO:
					snprintf(tmp, sizeof(tmp),
						"incompatible sub types: %s != %s",
						AudioFormats::getAudioFormatString(st1->af),
						AudioFormats::getAudioFormatString(st2->af));
					break;
				case DT_TEXT:
					snprintf(tmp, sizeof(tmp),
						"incompatible sub types: %s != %s",
						TextFormats::getTextFormatString(st1->tf),
						TextFormats::getTextFormatString(st2->tf));
					break;
				case DT_BYTES:
					snprintf(tmp, sizeof(tmp),
						"incompatible sub types: %s != %s",
						ByteFormats::getByteFormatString(st1->bf),
						ByteFormats::getByteFormatString(st2->bf));
					break;
				default:
					snprintf(tmp, sizeof(tmp),
						"strange, subtype set for generic pins.");
				}

				_error_string = tmp;
				return FAILURE;
			}
		}

		// connect the pins to this wire swapping them if needed
		if (p1->getDirection() == Pin::DIR_INPUT ||
			p2->getDirection() == Pin::DIR_OUTPUT) {
			// DEBUG
			UOSUTIL_DOUT(("Wire::connect(): swapping pin %s and pin %s\n",
				p1->getName(), p2->getName()));

			tmp_b1 = b2; _p1 = p2;
			tmp_b2 = b1; _p2 = p1;
		} else {
			tmp_b1 = b1; _p1 = p1;
			tmp_b2 = b2; _p2 = p2;
		}

		// associate this wire to each pin
		ret = _p1->_wires.pput(_p2->getAbsoluteName(), (char *) this);
		if (ret != SUCCESS) {
			_error_string = "cannot register wire for peer pin2";
			return FAILURE;
		}

		ret = _p2->_wires.pput(_p1->getAbsoluteName(), (char *) this);
		if (ret != SUCCESS) {
			_error_string = "cannot register wire for peer pin1";
			return FAILURE;
		}

		// connect the pins to each other
		ret = _p1->connect(tmp_b2, _p2);
		if (ret == FAILURE) {
			_error_string = "cannot connect pin1 to pin2";
			return FAILURE;
		}

		ret = _p2->connect(tmp_b1, _p1);
		if (ret == FAILURE) {
			_error_string = "cannot connect pin2 to pin1";
			return FAILURE;
		}

		// allocate buffers
		ret = _allocate();
		if (ret == FAILURE)
			return FAILURE;

		// ok
		return SUCCESS;
	}

	int32 Wire::_allocate(void)
	{
		uint32 max_buf_size = 0, p1_bsz = 0, p2_bsz = 0;
		int32 max_buf_count = 0, p1_bco = 0, p2_bco = 0;
		int32 ret = 0;

		char tmp[4096];

		// get right buffers info
		if (_p1->getStatus() == Pin::CONNECTED) {
			p1_bsz = _p1->_real_bufsz;
			p1_bco = _p1->_real_bufcount;
		} else {
			p1_bsz = _p1->_pref_bufsz;
			p1_bco = _p1->_pref_bufcount;
		}

		if (_p2->getStatus() == Pin::CONNECTED) {
			p2_bsz = _p2->_real_bufsz;
			p2_bco = _p2->_real_bufcount;
		} else {
			p2_bsz = _p2->_pref_bufsz;
			p2_bco = _p2->_pref_bufcount;
		}

		// select right buffers size and count
		max_buf_size = p1_bsz > p2_bsz ? p1_bsz : p2_bsz;
		max_buf_count = p1_bco > p2_bco ? p1_bco : p2_bco;

		// DEBUG
		UOSUTIL_DOUT(("Wire::_allocate(): BSZ = %u, BCO = %d\n", max_buf_size,
			max_buf_count));

		// lock pins
		MutexLocker ml1(_p1);
		MutexLocker ml2(_p2);

		// check which pin needs a buffer pool
		if (!_p1->_bpSet) {
			// check if this wire is bidirectional
			if (_type == BIDIRECTIONAL) {
				// build buffer pool name
				snprintf(tmp, sizeof(tmp), "BP1[%s,%s]",
					_p1->getAbsoluteName(), _p2->getAbsoluteName());

				// create buffer pool object
				_bp1 = new BufferPool();
				if (!_bp1)
					return FAILURE;

				// allocate buffer pool for Pin 1
				ret = _bp1->init(tmp, max_buf_size, max_buf_count);
				if (ret == FAILURE) {
					delete _bp1; return FAILURE;
				}

				// DEBUG
				UOSUTIL_DOUT(("Wire::_allocate(): allocating bp for pin %s\n",
					_p1->getAbsoluteName()));

				// associate bp1 to pin 1
				_p1->_ibp = _bp1;
			} else {
				// set input buffer pool for pin 1 to NULL
				_p1->_ibp = NULL;
			}

			// signal that buffer pool has been set
			_p1->_bpSet = true;
		} else {
			// use pin 1's buffer pool
			_bp1 = _p1->_ibp;
			_multipoint_on_bp1 = true;

			// DEBUG
			UOSUTIL_DOUT(("Wire::_allocate(): it is multipoint\n"));
			UOSUTIL_DOUT(("Wire::_allocate(): using buffer pool of pin %s (%s)\n",
				_p1->getAbsoluteName(), _bp1 ?
				"and is ok" :
				"and is null"));
		}

		/*
		 * Notice that data flow from pin1 to pin2. Buffer pool
		 * is associated to the input pin so I need to create
		 * a buffer pool only for pin2.
		 */

		if (!_p2->_bpSet) {
			// build buffer pool name
			snprintf(tmp, sizeof(tmp), "BP2[%s,%s]", _p1->getAbsoluteName(),
				_p2->getAbsoluteName());

			// create buffer pool object
			_bp2 = new BufferPool();
			if (!_bp2)
				return FAILURE;

			// allocate buffer pool for Pin 2
			ret = _bp2->init(tmp, max_buf_size, max_buf_count);
			if (ret == FAILURE) {
				delete _bp2; return FAILURE;
			}

			// associate bp2 to pin 2
			_p2->_ibp = _bp2;

			// signal that buffer pool has been set
			_p2->_bpSet = true;

			// DEBUG
			UOSUTIL_DOUT(("Wire::_allocate(): allocating bp for pin %s\n",
				_p2->getAbsoluteName()));
		} else {
			// use pin 2's buffer pool
			_bp2 = _p2->_ibp;
			_multipoint_on_bp2 = true;

			// DEBUG
			UOSUTIL_DOUT(("Wire::_allocate(): - it is multipoint\n"));
			UOSUTIL_DOUT(("Wire::_allocate(): using buffer pool of pin %s (%s)\n",
				_p2->getAbsoluteName(), _bp2 ?
				"and is ok" :
				"and is null"));
		}

		// set real parameters for pins
		_p1->_real_bufsz = max_buf_size;
		_p1->_real_bufcount = max_buf_count;
		_p2->_real_bufsz = max_buf_size;
		_p2->_real_bufcount = max_buf_count;

		// ok
		return SUCCESS;
	}
}
