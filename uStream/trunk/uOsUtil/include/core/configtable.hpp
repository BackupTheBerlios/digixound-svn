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

#ifndef CONFIGTABLE_HPP
#define CONFIGTABLE_HPP

#include "shared_hash.hpp"
#include "pvector.hpp"
#include "vector.hpp"
#include "callback.hpp"

namespace uStreamLib {
	/* Forward declaration of ConfigTable */
	class ConfigTable;

	/**
	 * The ConfigCallBack is a callback class that is invoked
	 * when it is attached to a specific parameter and the latter is
	 * read or written.
	 */
	class US_API_EXPORT ConfigCallBack : public CallBack {
	public:
		friend class ConfigTable;

		ConfigCallBack(void);
		virtual ~ConfigCallBack(void);

		int32 init(char* name);

		virtual int32 perform(void*)
		{
			return SUCCESS;
		}

		bool isEnabled(void)
		{
			return enabled;
		}

		void setEnabled(bool flag)
		{
			enabled = flag;
		}
	protected:
		/* pointer to ConfigTable object */
		ConfigTable* ct;

		/* key (property name) */
		char* key;

		/* property type */
		uint8 type;

		/* object type id */
		uint16 otype_id;

		/* the integer value for this key */
		int32* ival;

		/* the double value for this key */
		double* dval;

		/* the string value for this key */
		char* sval;

		/* size of the string value */
		uint32 svalsz;

		/* the object value for this key */
		void* oval;

		/* custom data pointer */
		void* userdata;

		/* flag: callback enabled or disabled */
		bool enabled;
	private:
		/* copy constructor not avilable */
		ConfigCallBack(ConfigCallBack&)
		{
		}
	};

	class US_API_EXPORT Property : public Object {
	public:
		/**
		 * Property types.
		 */
		enum { TYPE_UNDEF = 0, TYPE_INT = 1, TYPE_DOUBLE = 2, TYPE_STRING = 3,
		TYPE_OBJECT = 4 };

		/**
		 * Constructor.
		 */
		Property(void);

		/**
		 * Destructor.
		 */
		virtual ~Property(void);

		/**
		 * Build a Property description for a ConfigTable.
		 * Each property has a type (one of TYPE_INT, TYPE_DOUBLE,
		 * TYPE_STRING, TYPE_OBJECT or TYPE_UNDEF).
		 * A property can be an integer or double in a specific range
		 * or can allow a discrete domain of values. If getAllowedValues()
		 * returns a Vector on which getCount() is not 0, then the Property
		 * accept only the values contained in this vector.
		 * @param type one of type identifiers.
		 * @param allowed_size max size of an element in the allowed_values
		 * vector (default to 255); valid only for STRINGs and OBJECTs.
		 * @param description optional property description.
		 */
		int32 init(uint8 type, uint32 allowed_size = 255,
			char* description = NULL);

		/**
		 * Get property type.
		 * @return one of TYPE_UNDEF, TYPE_INT, TYPE_DOUBLE, TYPE_STRING
		 * or TYPE_OBJECT.
		 */
		uint8 getType(void)
		{
			return _type;
		}

		/**
		 * Get property description.
		 * @return a string containing the property description.
		 */
		char* getDescription(void)
		{
			return _dbDesc.toString();
		}

		/**
		 * Set property description.
		 * @param descr a string containing the description.
		 * @return SUCCESS or FAILURE if description cannot be set.
		 */
		int32 setDescription(char* descr)
		{
			return _dbDesc.xcopy(descr);
		}

		/**
		 * Get vector of allowed values.
		 * The vector contains data of type specified by the getType()
		 * method.
		 * @return a pointer to the vector.
		 */
		Vector* getAllowedValues(void)
		{
			return &_vAllowed;
		}

		/**
		 * Get allowed max integer.
		 * @return an integer.
		 */
		int32 getAllowedMaxInteger(void)
		{
			return _int_max;
		}

		/**
		 * Set allowed max integer.
		 * @param max an integer.
		 */
		void setAllowedMaxInteger(int32 max)
		{
			_int_max = max;
		}

		/**
		 * Get allowed min integer.
		 * @return an integer.
		 */
		int32 getAllowedMinInteger(void)
		{
			return _int_min;
		}

		/**
		 * Set allowed min integer.
		 * @param min an integer.
		 */
		void setAllowedMinInteger(int32 min)
		{
			_int_min = min;
		}

		/**
		 * Get allowed max double.
		 * @return a double.
		 */
		double getAllowedMaxDouble(void)
		{
			return _double_max;
		}

		/**
		 * Set allowed max double.
		 * @param max a double.
		 */
		void setAllowedMaxDouble(double max)
		{
			_double_max = max;
		}

		/**
		 * Get allowed min double.
		 * @return a double.
		 */
		double getAllowedMinDouble(void)
		{
			return _double_min;
		}

		/**
		 * Set allowed min double.
		 * @param min a double.
		 */
		void setAllowedMinDouble(double min)
		{
			_double_min = min;
		}

		/**
		 * Get label.
		 * @return the label.
		 */
		char* getLabel(void)
		{
			return _dbLabel.toString();
		}

		/**
		 * Set label.
		 * @param label the label.
		 */
		void setLabel(char* label)
		{
			_dbLabel.xcopy(label);
		}

		/**
		 * Check if this property is visible in the config panel.
		 * @return true or false.
		 */
		bool isVisible(void)
		{
			return _bVisible;
		}

		/**
		 * Set the visible flag.
		 * @param flag true means that this property will not be shown
		 * in the config panel.
		 */
		void setVisible(bool flag)
		{
			_bVisible = flag;
		}

		/**
		 * Check if this property is read only.
		 * @return true or false.
		 */
		bool isReadOnly(void)
		{
			return _bReadOnly;
		}

		/**
		 * Set the readonly flag.
		 * @param flag true means that this property will be read only
		 * in the config panel.
		 */
		void setReadOnly(bool flag)
		{
			_bReadOnly = flag;
		}

		/**
		 * Check if this property is secret (like a password).
		 * @return true or false.
		 */
		bool isSecret(void)
		{
			return _bSecret;
		}

		/**
		 * Set the secret flag.
		 * @param flag true means that this property will be secret
		 * in the config panel (secret == password).
		 */
		void setSecret(bool flag)
		{
			_bSecret = flag;
		}
	private:
		/* copy constructor not avilable */
		Property(Property&)
			: Object(UOSUTIL_RTTI_PROPERTY)
		{
		}

		/* property type */
		uint8 _type;

		/* integer range limits */
		int32 _int_max;
		int32 _int_min;

		/* double range limits */
		double _double_max;
		double _double_min;

		/* allowed values vector */
		Vector _vAllowed;

		/* string description */
		DataBuf _dbDesc;

		/* the label for this property */
		DataBuf _dbLabel;

		/* flag: the property is visible (default true) */
		bool _bVisible;

		/* flag: the property is read only (default false) */
		bool _bReadOnly;

		/* flag: the property is secret */
		bool _bSecret;
	};

	/**
	 * This class provides the concept of group of properties.
	 * You can create logical groups of properties so that
	 * a GUI can do its best layout in some way.
	 */
	class PropertyGroup : public Vector {
	public:
		/**
		 * Constructor.
		 */
		PropertyGroup(void);

		/**
		 * Destructor.
		 */
		~PropertyGroup(void);

		/**
		 * Build a property group with the specified name.
		 * @param name group name.
		 */
		int32 init(char* name, uint32 item_sz = 64);
	private:
		/* copy constructor not available */
		PropertyGroup(PropertyGroup&)
		{
		}

		/* group name */
		DataBuf _dbName;
	};

	/**
	 * This class provides a general way to store and retrieve
	 * properties in a key-value structure. This class uses an
	 * hash table (thread safe version) and provides methods to
	 * store and retrieve integers, strings, doubles and objects.
	 * Integers, strings and doubles are stored by value, whereas
	 * objects are stored as pointers.
	 */
	class US_API_EXPORT ConfigTable : public Object {
	public:
		/**
		 * Constructor.
		 */
		ConfigTable(void);

		/**
		 * Destructor.
		 * Strings are stored in this table, so this method will deallocate
		 * all used memory. Objects are stored as pointers.
		 * All callbacks will be deleted by this method by default.
		 */
		virtual ~ConfigTable(void);

		/**
		 * Create a Configuration Parameters Table. Parameters
		 * are stored into an hash table in the form of key,value pairs.
		 * Each parameter can have a type: TYPE_INT, TYPE_DOUBLE,
		 * TYPE_STRING or TYPE_OBJECT.
		 * @param hash_size the number of buckets in the hash table.
		 */
		int32 init(uint32 hash_size);

		/**
		 * Implement this method in subclasses if you want this config
		 * table be correctly managed by ConfigStorage objects. This
		 * method must return a valid filename. Please, don't provide
		 * a path but only a filename since ConfigTable provides 
		 * the full path.
		 */
		virtual char* getStorageFilename(void)
		{
			return _dbName.toString();
		}

		/**
		 * Set config table name for config storage.
		 */
		void setStorageFilename(char* name)
		{
			_dbName.xcopy(name);
		}

		/**
		 * Create extended property descriptor.
		 * This method allows to setup a complex property description
		 * for the specified property key. A property description is an
		 * object in which to store extended info for gui interaction
		 * and for registering configtables into ConfigStorage objects.
		 * Key must exist to add an extended property description.
		 * @param key the property key to use.
		 * @param allowed_size maximum size of an element in the allowed
		 * values vector (default = 255).
		 * @return a pointer to the property description or NULL if
		 * it cannot be created.
		 */
		Property* createPropertyDescription(char* key,
			uint32 allowed_size = 255);

		/**
		 * Get property description for the specified key.
		 * @param key the property whose description must be returned.
		 * @return a pointer to the property description or NULL if the
		 * key or property does not exist.
		 */
		Property* getPropertyDescription(char* key);

		/**
		 * Create a property group.
		 * This method creates a group of properties that logically binds
		 * properties together.
		 * @param gname property group name.
		 * @return a pointer to the created group or NULL if an error occurs.
		 */
		PropertyGroup* createPropertyGroup(char* name);

		/**
		 * Get a property group.
		 * @param gname property group name.
		 * @return a pointer to the property group or NULL if it does not exist.
		 */
		PropertyGroup* getPropertyGroup(char* name)
		{
			return (PropertyGroup *) _shGroups.get(name);
		}

		/**
		 * Get the selected key, assuming that its value is a string.
		 * On Read callbacks will be executed, if any.
		 * @param key the desired key.
		 * @param value a pointer to pointer for storing the returned value.
		 * @return SUCCESS or FAILURE if the key does not exist.
		 */
		int32 getString(char* key, char** value);

		/**
		 * Get the selected key, assuming that its value is an integer.
		 * On Read callbacks will be executed, if any.
		 * @param key the desired key.
		 * @param value a pointer to an integer for storing the returned value.
		 * @return SUCCESS or FAILURE if the key does not exist.
		 */
		int32 getInt(char* key, int32* value);

		/**
		 * Get the selected key, assuming that its value is a double.
		 * On Read callbacks will be executed, if any.
		 * @param key the desired key.
		 * @param value a pointer to a double for storing the returned value.
		 * @return SUCCESS or FAILURE if the key does not exist.
		 */
		int32 getDouble(char* key, double* value);

		/**
		 * Get the selected key, assuming that its value is an object.
		 * On Read callbacks will be executed, if any.
		 * @param key the desired key.
		 * @param value a pointer to a pointer for storing the returned value.
		 * @param otype_id a pointer to an uint16 for storing custom object type
		 * identifier (provided by the user by invoking setObject).
		 * @return SUCCESS or FAILURE if the key does not exist.
		 */
		int32 getObject(char* key, void** value, uint16* otype_id);

		/**
		 * Get the type of the desired parameter.
		 * @param key the desired parameter.
		 * @param type a pointer to an uint8 for storing type info.
		 * @return SUCCESS or FAILURE if parameter does not exist.
		 */
		int32 getParamType(char* key, uint8* type);

		/**
		 * Attach a callback to be invoked when reading a key.
		 * @param key the key to attach the callback to.
		 * @param ccb the callback object to attach.
		 * @param user_data generic user data to pass to the callback.
		 * @return FAILURE or the index of the callback in the vector.
		 */
		int32 attachRead(char* key, ConfigCallBack* ccb, void* user_data);

		/**
		 * Clear vector of attached callbacks for read operation.
		 * @param key the key whose vector will be cleared.
		 * @param dealloc true means make a delete of all callbacks.
		 * @return SUCCESS or FAILURE.
		 */
		int32 clearRead(char* key, bool dealloc = true);

		/**
		 * Attach a callback to be invoked when writing a key.
		 * @param key the key to attach the callback to.
		 * @param ccb the callback object to attach.
		 * @param user_data generic user data to pass to the callback.
		 * @return FAILURE or the index of the callback in the vector.
		 */
		int32 attachWrite(char* key, ConfigCallBack* ccb, void* user_data);

		/**
		 * Clear vector of attached callbacks for write operation.
		 * @param key the key whose vector will be cleared.
		 * @param dealloc true means make a delete of all callbacks.
		 * @return SUCCESS or FAILURE.
		 */
		int32 clearWrite(char* key, bool dealloc = true);

		/**
		 * Register and set the specified key. Its value is an integer.
		 * Use this method to register the parameter for the first time.
		 * Callbacks will be invoked on this write operation.
		 * @param key he key to register.
		 * @param value the value to store.
		 * @return SUCCESS or FAILURE.
		 */
		int32 setInt(char* key, int32 value);

		/**
		 * Register and set the specified key. Its value is a double.
		 * Use this method to register the parameter for the first time.
		 * Callbacks will be invoked on this write operation.
		 * @param key he key to register.
		 * @param value the value to store.
		 * @return SUCCESS or FAILURE.
		 */
		int32 setDouble(char* key, double value);

		/**
		 * Register and set the specified key. Its value is a string.
		 * Use this method to register the parameter for the first time.
		 * Callbacks will be invoked on this write operation.
		 * @param key he key to register.
		 * @param value the value to store.
		 * @param val_size size of the specified string.
		 * @return SUCCESS or FAILURE.
		 */
		int32 setString(char* key, char* value, uint32 val_size);

		/**
		 * Register and set the specified key. Its value is an object.
		 * Use this method to register the parameter for the first time.
		 * Callbacks will be invoked on this write operation.
		 * @param key he key to register.
		 * @param value the value to store (only the pointer is stored).
		 * @param otype_id custom object type identifier.
		 * @return SUCCESS or FAILURE.
		 */
		int32 setObject(char* key, void* object, uint16 otype_id);

		/**
		 * Set a property to the database like null value.
		 * If a property is set to null, then a getInt(), getString(),
		 * getDouble() and getObject() on it will return FAILURE.
		 * A call to setInt(), setString(), setDouble() and setObject()
		 * on it will make the property visible again.
		 */
		int32 setNull(char* key);

		/**
		 * Get all properties.
		 * @return an enumeration of property keys.
		 */
		Enumeration* keys(void)
		{
			return _shDtable.keys();
		}

		/**
		 * Get all property groups.
		 * @return an enumeration of property groups.
		 */
		Enumeration* groups(void)
		{
			return _shGroups.values();
		}

		/**
		 * Clear config table.
		 * Remove all keys, properties, config callbacks.
		 * Please notice that objects will not be deleted.
		 */
		void clear(void);
	private:
		// copy constructor not available
		ConfigTable(ConfigTable&)
			: Object(UOSUTIL_RTTI_CONFIG_TABLE)
		{
		}

		// property descriptor
		struct pdesc_t {
			// optional property descriptor
			Property* property;

			// property type
			uint8 type;

			// object type id
			uint16 otype_id;

			// vector of on read callbacks
			PVector* on_readv;

			// vector of on write callbacks
			PVector* on_writev;

			// user data to pass to callbacks
			void* user_data;

			// value is null
			uint32 is_null;
		};

		// configuration parameter values
		SharedHash _shVtable;

		// configuration parameter descriptors
		SharedHash _shDtable;

		// configuration parameters groups
		SharedHash _shGroups;

		// config table name for storage
		DataBuf _dbName;

		/*
		 * Invoke this method for deleting property descriptors.
		 */
		void _destroyPropertyDescriptions(void);
	};
}

#endif
