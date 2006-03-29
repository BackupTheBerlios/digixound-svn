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

#include "configtable.hpp"

namespace uStreamLib {
	/*
	 * Implementation of ConfigCallBack.
	 */

	ConfigCallBack::ConfigCallBack(void)
		: ct(NULL), key(NULL), type(0), otype_id(0), ival(NULL), dval(NULL),
		sval(NULL), svalsz(0), oval(NULL), userdata(NULL), enabled(true)
	{
		setClassID(UOSUTIL_RTTI_CONFIG_CALLBACK);
	}

	int32 ConfigCallBack::init(char* name)
	{
		// initialize parent
		return CallBack::init(name);
	}

	ConfigCallBack::~ConfigCallBack(void)
	{
		// nothing to do
	}

	/*
	 * Implementation of Property.
	 */

	Property::Property(void)
		: Object(UOSUTIL_RTTI_PROPERTY), _type(TYPE_UNDEF), _int_max(0),
		_int_min(0), _double_max(0.0), _double_min(0.0), _bVisible(true),
		_bReadOnly(false), _bSecret(false)
	{
		// nothing to do
	}

	Property::~Property(void)
	{
		// nothing to do
	}

	int32 Property::init(uint8 type, uint32 allowed_size, char* description)
	{
		int32 ret = 0;

		// initialize properties
		_type = type;
		_int_max = 0;
		_int_min = 0;
		_double_max = 0.0;
		_double_min = 0.0;
		_bVisible = true;
		_bReadOnly = false;
		_bSecret = false;

		// allocate "allowed values" vector
		switch (type) {
		case TYPE_INT:
			ret = _vAllowed.init(sizeof(int32));
			if (ret == FAILURE)
				return FAILURE;
			break;
		case TYPE_DOUBLE:
			ret = _vAllowed.init(sizeof(double));
			if (ret == FAILURE)
				return FAILURE;
			break;
		case TYPE_STRING:
			ret = _vAllowed.init(allowed_size);
			if (ret == FAILURE)
				return FAILURE;
			break;
		case TYPE_OBJECT:
			ret = _vAllowed.init(allowed_size);
			if (ret == FAILURE)
				return FAILURE;
			break;
		default:
			break;
		}

		// allocate description buffer
		if (description) {
			ret = _dbDesc.init(description);
			if (ret == FAILURE)
				return FAILURE;
		} else {
			ret = _dbDesc.init("");
			if (ret == FAILURE)
				return FAILURE;
		}

		// allocate label buffer
		ret = _dbLabel.init("");
		if (ret == FAILURE)
			return FAILURE;

		// ok
		setOk(true);
		return SUCCESS;
	}

	/*
	* Implementation of PropertyGroup.
	*/

	PropertyGroup::PropertyGroup(void)
	{
		setClassID(UOSUTIL_RTTI_PROPERTY_GROUP);
	}

	PropertyGroup::~PropertyGroup(void)
	{
		// nothing to do
	}

	int32 PropertyGroup::init(char* name, uint32 item_sz)
	{
		int32 ret = 0;

		// initialize parent
		ret = Vector::init(item_sz);
		if (ret == FAILURE)
			return FAILURE;

		// initialize name
		ret = _dbName.init(name);
		if (ret == FAILURE)
			return FAILURE;

		// ok
		setOk(true);
		return SUCCESS;
	}

	/*
	* Implementation of ConfigTable.
	*/

	ConfigTable::ConfigTable(void)
		: Object(UOSUTIL_RTTI_CONFIG_TABLE)
	{
		// nothing to do
	}

	ConfigTable::~ConfigTable(void)
	{
		// destroy property descriptions
		_destroyPropertyDescriptions();
	}

	int32 ConfigTable::init(uint32 hash_size)
	{
		int32 ret = 0;

		// initialize tables
		ret = _shVtable.init(hash_size);
		if (ret == FAILURE)
			return FAILURE;

		ret = _shDtable.init(hash_size);
		if (ret == FAILURE)
			return FAILURE;

		ret = _shGroups.init(hash_size);
		if (ret == FAILURE)
			return FAILURE;

		// initialize name
		ret = _dbName.init("ConfigTable");
		if (ret == FAILURE)
			return FAILURE;

		// ok
		setOk(true);
		return SUCCESS;
	}

	void ConfigTable::_destroyPropertyDescriptions(void)
	{
		Enumeration* en = NULL, * ev = NULL;
		char* key = NULL;

		// delete property objects
		en = _shDtable.values();
		while (en->hasMoreElements()) {
			struct pdesc_t* pd = (struct pdesc_t*) en->nextElement();
			if (pd->property) {
				// DEBUG
				UOSUTIL_DOUT(("ConfigTable::destroyPropertyDescriptions(): deleting \"%s\"\n",
					pd->property->getDescription()));
				delete pd->property;
			}
		}

		// DEBUG
		UOSUTIL_DOUT(("ConfigTable::destroyPropertyDescriptions(): removing callbacks...\n"));

		// delete all callback objects
		en = _shVtable.keys();
		while (en->hasMoreElements()) {
			char* key = (char*) en->nextElement();
			UOSUTIL_DOUT(("ConfigTable::destroyPropertyDescriptions(): key = %s\n",
				key));

			clearRead(key, true);
			clearWrite(key, true);
		}

		// delete callback vectors
		ev = _shDtable.values();
		while (ev->hasMoreElements()) {
			struct pdesc_t* pd = (struct pdesc_t*) ev->nextElement();
			if (pd->on_readv)
				delete pd->on_readv;
			if (pd->on_writev)
				delete pd->on_writev;
		}

		// empty property hash tables
		en = _shDtable.keys();
		while (en->hasMoreElements()) {
			key = (char *) en->nextElement();
			_shDtable.del(key);
		}

		en = _shVtable.keys();
		while (en->hasMoreElements()) {
			key = (char *) en->nextElement();
			_shVtable.del(key);
		}

		// delete property groups
		en = _shGroups.keys();
		while (en->hasMoreElements()) {
			char* gkey = (char*) en->nextElement();
			UOSUTIL_DOUT(("ConfigTable::destroyPropertyDescriptions(): group = %s\n",
				gkey));

			delete ((PropertyGroup *) _shGroups.get(gkey));
			_shGroups.del(gkey);
		}

		// DEBUG
		UOSUTIL_DOUT(("ConfigTable::destroyPropertyDescriptions(): ok\n"));
	}

	Property* ConfigTable::createPropertyDescription(char* key,
		uint32 allowed_size)
	{
		Property* prop = NULL;
		struct pdesc_t* pd = NULL;
		int32 ret = 0;

		pd = (struct pdesc_t *) _shDtable.get(key);
		if (!pd)
			return NULL;

		prop = new Property();
		pd->property = prop;

		ret = prop->init(pd->type, allowed_size);
		if (ret == FAILURE) {
			delete prop; return NULL;
		}

		// DEBUG
		UOSUTIL_DOUT(("createProperty(): %s\n", prop->getDescription()));

		return prop;
	}

	Property* ConfigTable::getPropertyDescription(char* key)
	{
		struct pdesc_t* pd = NULL;

		pd = (struct pdesc_t *) _shDtable.get(key);
		if (!pd)
			return NULL;

		// DEBUG
		UOSUTIL_DOUT(("getProperty(): found for key \"%s\"\n", key));

		return pd->property;
	}

	PropertyGroup* ConfigTable::createPropertyGroup(char* name)
	{
		PropertyGroup* pg = NULL;
		int32 ret = 0;

		pg = new PropertyGroup();

		ret = pg->init(name);
		if (ret == FAILURE) {
			delete pg; return NULL;
		}

		ret = _shGroups.pput(name, (char *) pg);
		if (ret != SUCCESS) {
			delete pg; return NULL;
		}

		return pg;
	}

	int32 ConfigTable::getString(char* key, char** value)
	{
		struct pdesc_t* pd = NULL;
		int32 res = FAILURE;
		char* ret;

		pd = (struct pdesc_t *) _shDtable.get(key);
		if (!pd)
			return FAILURE;

		if (pd->type != Property::TYPE_STRING)
			return FAILURE;
		if (pd->is_null)
			return FAILURE;

		ret = _shVtable.get(key);
		if (ret) {
			*value = ret; res = SUCCESS;
		}

		for (uint32 i = 0; i < pd->on_readv->getCount(); i++) {
			ConfigCallBack* ccb = (ConfigCallBack*) (*pd->on_readv)[i];

			// DEBUG
			UOSUTIL_DOUT(("getString(): invoking callback %s\n", ccb->getName()));

			ccb->ct = this;
			ccb->key = key;
			ccb->type = Property::TYPE_STRING;
			ccb->otype_id = 0;
			ccb->ival = NULL;
			ccb->dval = NULL;
			ccb->sval = ret;
			ccb->oval = NULL;
			ccb->svalsz = (uint32) strlen(ret) + 1;
			ccb->userdata = pd->user_data;

			if (ccb->isEnabled())
				ccb->perform(pd->user_data);
		}

		return res;
	}

	int32 ConfigTable::getInt(char* key, int32* value)
	{
		struct pdesc_t* pd = NULL;
		int32 res = FAILURE;
		int32* ret;

		pd = (struct pdesc_t *) _shDtable.get(key);
		if (!pd)
			return FAILURE;

		if (pd->type != Property::TYPE_INT)
			return FAILURE;
		if (pd->is_null)
			return FAILURE;

		ret = (int32 *) _shVtable.get(key);
		if (ret) {
			*value = *ret; res = SUCCESS;
		}

		for (uint32 i = 0; i < pd->on_readv->getCount(); i++) {
			ConfigCallBack* ccb = (ConfigCallBack*) (*pd->on_readv)[i];

			// DEBUG
			UOSUTIL_DOUT(("getInt(): invoking callback %s\n", ccb->getName()));

			ccb->ct = this;
			ccb->key = key;
			ccb->type = Property::TYPE_INT;
			ccb->otype_id = 0;
			ccb->ival = ret;
			ccb->dval = NULL;
			ccb->sval = NULL;
			ccb->oval = NULL;
			ccb->svalsz = 0;
			ccb->userdata = pd->user_data;

			if (ccb->isEnabled())
				ccb->perform(pd->user_data);
		}

		return res;
	}

	int32 ConfigTable::getDouble(char* key, double* value)
	{
		struct pdesc_t* pd = NULL;
		int32 res = FAILURE;
		double* ret;

		pd = (struct pdesc_t *) _shDtable.get(key);
		if (!pd)
			return FAILURE;

		if (pd->type != Property::TYPE_DOUBLE)
			return FAILURE;
		if (pd->is_null)
			return FAILURE;

		ret = (double *) _shVtable.get(key);
		if (ret) {
			*value = *ret; res = SUCCESS;
		}

		for (uint32 i = 0; i < pd->on_readv->getCount(); i++) {
			ConfigCallBack* ccb = (ConfigCallBack*) (*pd->on_readv)[i];

			// DEBUG
			UOSUTIL_DOUT(("getDouble(): invoking callback %s\n", ccb->getName()));

			ccb->ct = this;
			ccb->key = key;
			ccb->type = Property::TYPE_DOUBLE;
			ccb->otype_id = 0;
			ccb->ival = NULL;
			ccb->dval = ret;
			ccb->sval = NULL;
			ccb->oval = NULL;
			ccb->svalsz = 0;
			ccb->userdata = pd->user_data;

			if (ccb->isEnabled())
				ccb->perform(pd->user_data);
		}

		return res;
	}

	int32 ConfigTable::getObject(char* key, void** value, uint16* otype_id)
	{
		struct pdesc_t* pd = NULL;
		int32 res = FAILURE;
		char* ret;

		pd = (struct pdesc_t *) _shDtable.get(key);
		if (!pd)
			return FAILURE;

		if (pd->type != Property::TYPE_OBJECT)
			return FAILURE;
		if (pd->is_null)
			return FAILURE;

		ret = _shVtable.get(key);
		if (ret) {
			*value = ret; *otype_id = pd->otype_id; res = SUCCESS;
		}

		for (uint32 i = 0; i < pd->on_readv->getCount(); i++) {
			ConfigCallBack* ccb = (ConfigCallBack*) (*pd->on_readv)[i];

			// DEBUG
			UOSUTIL_DOUT(("getString(): invoking callback %s\n", ccb->getName()));

			ccb->ct = this;
			ccb->key = key;
			ccb->type = Property::TYPE_OBJECT;
			ccb->otype_id = pd->otype_id;
			ccb->ival = NULL;
			ccb->dval = NULL;
			ccb->sval = NULL;
			ccb->oval = ret;
			ccb->svalsz = 0;
			ccb->userdata = pd->user_data;

			if (ccb->isEnabled())
				ccb->perform(pd->user_data);
		}

		return res;
	}

	int32 ConfigTable::getParamType(char* key, uint8* type)
	{
		struct pdesc_t* pd = NULL;

		pd = (struct pdesc_t *) _shDtable.get(key);
		if (!pd)
			return FAILURE;

		*type = pd->type; return SUCCESS;
	}

	int32 ConfigTable::attachRead(char* key, ConfigCallBack* ccb,
		void* user_data)
	{
		struct pdesc_t* pd = NULL;

		pd = (struct pdesc_t *) _shDtable.get(key);
		if (!pd)
			return FAILURE;

		// DEBUG
		UOSUTIL_DOUT(("attachRead(): adding callback %s\n", ccb->getName()));

		pd->user_data = user_data;
		return pd->on_readv->add(ccb);
	}

	int32 ConfigTable::clearRead(char* key, bool dealloc)
	{
		struct pdesc_t* pd = NULL;

		pd = (struct pdesc_t *) _shDtable.get(key);
		if (!pd || !pd->on_readv)
			return FAILURE;

		if (dealloc) {
			for (uint32 i = 0; i < pd->on_readv->getCount(); i++) {
				ConfigCallBack* ccb = (ConfigCallBack*) (*pd->on_readv)[i];

				// DEBUG
				UOSUTIL_DOUT(("ConfigTable::clearRead(): callback %s\n",
					ccb->getName()));

				if (ccb)
					delete ccb;
			}
		}

		pd->on_readv->clear(); return SUCCESS;
	}

	int32 ConfigTable::attachWrite(char* key, ConfigCallBack* ccb,
		void* user_data)
	{
		struct pdesc_t* pd = NULL;

		pd = (struct pdesc_t *) _shDtable.get(key);
		if (!pd)
			return FAILURE;

		// DEBUG
		UOSUTIL_DOUT(("attachWrite(): adding callback %s\n", ccb->getName()));

		pd->user_data = user_data;
		return pd->on_writev->add(ccb);
	}

	int32 ConfigTable::clearWrite(char* key, bool dealloc)
	{
		struct pdesc_t* pd = NULL;

		pd = (struct pdesc_t *) _shDtable.get(key);
		if (!pd || !pd->on_writev)
			return FAILURE;

		if (dealloc) {
			for (uint32 i = 0; i < pd->on_writev->getCount(); i++) {
				ConfigCallBack* ccb = (ConfigCallBack*) (*pd->on_writev)[i];

				// DEBUG
				UOSUTIL_DOUT(("ConfigTable::clearWrite(): callback %s\n",
					ccb->getName()));

				if (ccb)
					delete ccb;
			}
		}

		pd->on_writev->clear(); return SUCCESS;
	}

	int32 ConfigTable::setNull(char* key)
	{
		struct pdesc_t pd, * pdp = NULL;
		int32 ret;

		pd.type = Property::TYPE_INT;
		pd.property = NULL;
		pd.is_null = 1;

		ret = _shDtable.put(key, (char *) &pd, sizeof(pd));
		if (ret == FAILURE)
			return FAILURE;

		return SUCCESS;
	}

	int32 ConfigTable::setInt(char* key, int32 value)
	{
		struct pdesc_t pd, * pdp = NULL;
		int32 ret;

		pd.type = Property::TYPE_INT;
		pd.property = NULL;
		pd.is_null = 0;

		ret = _shDtable.put(key, (char *) &pd, sizeof(pd));
		if (ret == FAILURE)
			return FAILURE;

		pdp = (struct pdesc_t *) _shDtable.get(key);
		if (ret == SUCCESS) {
			pdp->on_readv = new PVector();
			ret = pdp->on_readv->init(sizeof(ConfigCallBack *));
			if (ret == FAILURE)
				return FAILURE;

			pdp->on_writev = new PVector();
			ret = pdp->on_writev->init(sizeof(ConfigCallBack *));
			if (ret == FAILURE)
				return FAILURE;
		}

		ret = _shVtable.put(key, (char *) &value, sizeof(int32), true);
		if (ret == FAILURE)
			return FAILURE;

		for (uint32 i = 0; i < pdp->on_writev->getCount(); i++) {
			ConfigCallBack* ccb = (ConfigCallBack*) (*pdp->on_writev)[i];

			// DEBUG
			UOSUTIL_DOUT(("setInt(): invoking callback %s\n", ccb->getName()));

			ccb->ct = this;
			ccb->key = key;
			ccb->type = Property::TYPE_INT;
			ccb->otype_id = 0;
			ccb->ival = &value;
			ccb->dval = NULL;
			ccb->sval = NULL;
			ccb->oval = NULL;
			ccb->svalsz = 0;
			ccb->userdata = pdp->user_data;

			if (ccb->isEnabled())
				ccb->perform(pdp->user_data);
		}

		return ret;
	}

	int32 ConfigTable::setDouble(char* key, double value)
	{
		struct pdesc_t pd, * pdp = NULL;
		int32 ret;

		pd.type = Property::TYPE_DOUBLE;
		pd.property = NULL;
		pd.is_null = 0;

		ret = _shDtable.put(key, (char *) &pd, sizeof(pd));
		if (ret == FAILURE)
			return FAILURE;

		pdp = (struct pdesc_t *) _shDtable.get(key);
		if (ret == SUCCESS) {
			pdp->on_readv = new PVector();
			ret = pdp->on_readv->init(sizeof(ConfigCallBack *));
			if (ret == FAILURE)
				return FAILURE;

			pdp->on_writev = new PVector();
			ret = pdp->on_writev->init(sizeof(ConfigCallBack *));
			if (ret == FAILURE)
				return FAILURE;
		}

		ret = _shVtable.put(key, (char *) &value, sizeof(double), true);
		if (ret == FAILURE)
			return FAILURE;

		for (uint32 i = 0; i < pdp->on_writev->getCount(); i++) {
			ConfigCallBack* ccb = (ConfigCallBack*) (*pdp->on_writev)[i];

			// DEBUG
			UOSUTIL_DOUT(("setDouble(): invoking callback %s\n", ccb->getName()));

			ccb->ct = this;
			ccb->key = key;
			ccb->type = Property::TYPE_DOUBLE;
			ccb->otype_id = 0;
			ccb->ival = NULL;
			ccb->dval = &value;
			ccb->sval = NULL;
			ccb->oval = NULL;
			ccb->svalsz = 0;
			ccb->userdata = pdp->user_data;

			if (ccb->isEnabled())
				ccb->perform(pdp->user_data);
		}

		return ret;
	}

	int32 ConfigTable::setString(char* key, char* value, uint32 val_size)
	{
		struct pdesc_t pd, * pdp = NULL;
		int32 ret;

		pd.type = Property::TYPE_STRING;
		pd.property = NULL;
		pd.is_null = 0;

		ret = _shDtable.put(key, (char *) &pd, sizeof(pd));
		if (ret == FAILURE)
			return FAILURE;

		pdp = (struct pdesc_t *) _shDtable.get(key);
		if (ret == SUCCESS) {
			pdp->on_readv = new PVector();
			ret = pdp->on_readv->init(sizeof(ConfigCallBack *));
			if (ret == FAILURE)
				return FAILURE;

			pdp->on_writev = new PVector();
			ret = pdp->on_writev->init(sizeof(ConfigCallBack *));
			if (ret == FAILURE)
				return FAILURE;
		}

		ret = _shVtable.put(key, value, val_size, true);
		if (ret == FAILURE)
			return FAILURE;

		for (uint32 i = 0; i < pdp->on_writev->getCount(); i++) {
			ConfigCallBack* ccb = (ConfigCallBack*) (*pdp->on_writev)[i];

			// DEBUG
			UOSUTIL_DOUT(("setString(): invoking callback %s\n", ccb->getName()));

			ccb->ct = this;
			ccb->key = key;
			ccb->type = Property::TYPE_STRING;
			ccb->otype_id = 0;
			ccb->ival = NULL;
			ccb->dval = NULL;
			ccb->sval = value;
			ccb->oval = NULL;
			ccb->svalsz = val_size;
			ccb->userdata = pdp->user_data;

			if (ccb->isEnabled())
				ccb->perform(pdp->user_data);
		}

		return ret;
	}

	int32 ConfigTable::setObject(char* key, void* object, uint16 otype_id)
	{
		struct pdesc_t pd, * pdp = NULL;
		int32 ret;

		pd.type = Property::TYPE_OBJECT;
		pd.otype_id = otype_id;
		pd.property = NULL;
		pd.is_null = 0;

		ret = _shDtable.put(key, (char *) &pd, sizeof(pd));
		if (ret == FAILURE)
			return FAILURE;

		pdp = (struct pdesc_t *) _shDtable.get(key);
		if (ret == SUCCESS) {
			pdp->on_readv = new PVector();
			ret = pdp->on_readv->init(sizeof(ConfigCallBack *));
			if (ret == FAILURE)
				return FAILURE;

			pdp->on_writev = new PVector();
			ret = pdp->on_writev->init(sizeof(ConfigCallBack *));
			if (ret == FAILURE)
				return FAILURE;
		}

		ret = _shVtable.pput(key, (char *) object, true);
		if (ret == FAILURE)
			return FAILURE;

		for (uint32 i = 0; i < pdp->on_writev->getCount(); i++) {
			ConfigCallBack* ccb = (ConfigCallBack*) (*pdp->on_writev)[i];

			// DEBUG
			UOSUTIL_DOUT(("setObject(): invoking callback %s\n", ccb->getName()));

			ccb->ct = this;
			ccb->key = key;
			ccb->type = Property::TYPE_OBJECT;
			ccb->otype_id = otype_id;
			ccb->ival = NULL;
			ccb->dval = NULL;
			ccb->sval = NULL;
			ccb->oval = object;
			ccb->svalsz = 0;
			ccb->userdata = pdp->user_data;

			if (ccb->isEnabled())
				ccb->perform(pdp->user_data);
		}

		return ret;
	}

	void ConfigTable::clear(void)
	{
		Enumeration* en = _shDtable.keys();
		while (en->hasMoreElements())
			setNull((char *) en->nextElement());
	}
}
