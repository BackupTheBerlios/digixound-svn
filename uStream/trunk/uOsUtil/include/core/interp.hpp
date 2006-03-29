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

#ifndef INTERP_HPP
#define INTERP_HPP

#include "shared_hash.hpp"
#include "databuf.hpp"
#include "callback.hpp"

namespace uStreamLib {
	/**
	 * Class to manage command parameters.
	 * This class is used to collect string parameters
	 * to be passed to an Interp's callback.
	 */
	class US_API_EXPORT Params : public Object {
	public:
		/**
		 * Constructor.
		 */
		Params(void);

		/**
		 * Destructor.
		 */
		virtual ~Params(void);

		/**
		 * Build params object.
		 * @param max_param_no maximum number of parameters to accept
		 * @param max_param_size maximum size of each parameter
		 */
		int32 init(uint32 max_param_no, uint32 max_param_size);

		/**
		 * Add the specified parameter to this Params table.
		 * @param param the parameter to add. Notice that the
		 * parameter cannot be longer than max_param_size.
		 * @return SUCCESS or FAILURE if param cannot be added.
		 */
		int32 add(char* param);

		/**
		 * Use operator [i] to get the parameter at the index i.
		 * In a Command callback, you can access the parameters table
		 * by using the "p" pointer (first argument). Given the p pointer
		 * you can get the parameter at position "i" by using the following
		 * syntax: (*p)[i]
		 */
		char* operator[](uint32 index)
		{
			return (index < m_uParamsFound ? m_strParam[index] : NULL);
		}

		/**
		 * Get parameters count in the table.
		 * @return count of parameters.
		 */
		uint32 getCount(void)
		{
			return m_uParamsFound;
		}

		/**
		 * Reset parameters table, ie empty the table.
		 */
		void reset(void)
		{
			m_uParamsFound = 0;
		}

		/**
		 * Set executing command name. Each command has
		 * a parameters table.
		 * @param cmdname command name.
		 * @return SUCCESS or FAILURE if command name cannot be set.
		 */
		int32 setCommandName(char* cmdname)
		{
			return m_dbCmdName.xcopy(cmdname);
		}

		/**
		 * Get current command name.
		 * @return the command name this parameters table is associated to.
		 */
		char* getCommandName(void)
		{
			return m_dbCmdName.toString();
		}
	private:
		/* copy constructor not available */
		Params(Params&)
			: Object(UOSUTIL_RTTI_PARAMS)
		{
		}

		/* command name */
		DataBuf m_dbCmdName;

		/* ptr to a string ptrs array */
		char** m_strParam;

		/* number of parameters found */
		uint32 m_uParamsFound;

		/* max parameters number */
		uint32 m_uMaxParamsNo;

		/* max parameter length */
		uint32 m_uMaxParamSize;
	};

	/**
	 * An Interp's Command.
	 * This class is the CallBack that Interp calls to execute
	 * the command string this callback is associated to.
	 */
	class US_API_EXPORT Command : public CallBack {
	public:
		/*
		 * Friend classes.
		 */
		friend class Interp;

		/**
		 * Constructor.
		 */
		Command(void);

		/**
		 * Destructor.
		 */
		virtual ~Command(void);

		/**
		 * Build an Interp's Command.
		 * @param name command's name.
		 * @param minpc minimum acceptable parameters count.
		 * @param maxpc maximum acceptable parameters count.
		 * @param context user data on which this command works on.
		 */
		int32 init(
			char* name, // command name
			uint32 min_pc, // minimum acceptable parameters count
			uint32 max_pc, // maximum acceptable parameters count
			void* context   // generic user data pointer
		);

		/**
		 * Check if this command is enabled.
		 * @return true if enabled, false otherwise.
		 */
		bool isEnabled(void)
		{
			return m_bEnabled;
		}

		/**
		 * Enable or disable this command.
		 * @param flag true to enable or false to disable.
		 */
		void setEnabled(bool flag)
		{
			m_bEnabled = flag;
		}

		/**
		 * Check parameters count.
		 * @param count number of detected parameters.
		 * @return true if min_param_count <= count <= max_param_count.
		 */
		bool checkParamsCount(uint32 count)
		{
			if (count < m_uMinParamsCount || count> m_uMaxParamsCount)
				return false;

			return true;
		}

		/**
		 * Get command's syntax string.
		 * @return a pointer to the syntax string.
		 */
		char* getSyntax(void)
		{
			return m_dbSyntax.toString();
		}

		/**
		 * Set command's syntax string.
		 * @param the syntax string as a byte array.
		 * @return SUCCESS or FAILURE if out of memory.
		 */
		int32 setSyntax(char* syntax)
		{
			return m_dbSyntax.xcopy(syntax);
		}

		/**
		 * Get command's description string.
		 * @return a pointer to the description string.
		 */
		char* getDescription(void)
		{
			return m_dbDesc.toString();
		}

		/**
		 * Set command's description string.
		 * @param the syntax string as a byte array.
		 * @return SUCCESS or FAILURE if out of memory.
		 */
		int32 setDescription(char* desc)
		{
			return m_dbDesc.xcopy(desc);
		}

	protected:
		/**
		 * Parameters maker. Method invoked for building parameters object to
		 * pass to Command. Override this method if you need to parse parameter
		 * string in a different way.
		 * @param p the Params object.
		 * @param instr_string the complete command string with
		 * parameters.
		 */
		virtual int32 makeParams(Params* p, char* instr_string);

		/**
		 * This method is the entry point for the Command.
		 * Override this method to make the command do something useful
		 * for you.
		 * @param p the parameters table.
		 * @param context the execution context for this command.
		 */
		virtual int32 perform(Params*, void*)
		{
			return SUCCESS;
		}

	private:
		/* copy constructor not available */
		Command(Command&)
		{
		}

		/* user data this callback works on */
		void* m_voidCtx;

		/* flag: command enabled or disabled */
		bool m_bEnabled;

		/* minimum acceptable parameters count */
		uint32 m_uMinParamsCount;

		/* maximum acceptable parameters count */
		uint32 m_uMaxParamsCount;

		/* command syntax string */
		DataBuf m_dbSyntax;

		/* command description string */
		DataBuf m_dbDesc;

		/* work buffer */
		DataBuf m_dbWork;
	};

	/**
	 * Class to manage a simple virtual machine.
	 * Based upon hash and callbacks.
	 *
	 * All callbacks must be allocated into the heap (using "new" operator).
	 * When deleted, the Interp does not free callbacks so that you must
	 * do it on your own to avoid memory leaks.
	 */
	class US_API_EXPORT Interp : public SharedHash, public Params {
	public:
		/**
		 * Run method's return values.
		 */
		enum InterpError {
			/** the command is not registred */
			BADCOMMAND = 0xffffffff,
			/** error while parsing parameters */
			PARAMPARSINGERROR = 0xfffffffe,
			/** the command was passed wrong parameters count */
			WRONGPARAMCOUNT = 0xfffffffd,
			/** the command was disabled */
			PERMISSIONDENIED = 0xfffffffc };

		/**
		 * Constructor.
		 */
		Interp(void);

		/**
		 * Destructor.
		 */
		virtual ~Interp(void);

		/**
		 * Create an Interp object.
		 * @param table_size initial hash buckets count.
		 * @param max_param_no maximum number of parameters per instruction.
		 * @param max_param_size maximum size of one parameter.
		 */
		int32 init(uint32 table_size, uint32 max_param_no,
			uint32 max_param_size);

		/**
		 * Add a Command to this Interp.
		 * @param c the command to add.
		 * @return SUCCESS or FAILURE if the command cannot be added.
		 */
		int32 addCommand(Command* c);

		/**
		 * Remove specified command.
		 */
		int32 delCommand(char* name);

		/**
		 * Get requested command.
		 * @param name command name.
		 * @return a pointer to a Command object.
		 */
		Command* getCommand(char* name);

		/**
		 * Get all commands.
		 */
		Enumeration* getCommands(void)
		{
			return values();
		}

		/**
		 * Enable or disable the specified command.
		 * @param name command name.
		 * @param enable true or false to enable/disable.
		 * @return SUCCESS or FAILURE if command cannot be found.
		 */
		int32 enableCommand(char* name, bool enable);

		/**
		 * Enable or disable parameters count check.
		 * When executing a command, the interp can check if parameters
		 * count of the current command is compatible with the one
		 * registered into the commands table.
		 * @param flag true or false to enable or disable this check.
		 */
		void setParamsCountCheck(bool flag);

		/**
		 * Use this method to delete all commands from table.
		 */
		void deleteCommands(void);

		/**
		 * Execute the specified command.
		 * This method executes the command whose name is "name", passing
		 * the "param_string" parameters string to the command callback.
		 * @param name command name.
		 * @param param_string parameter string.
		 * @param context an opaque pointer to the execution context of
		 * the commands.
		 * @return one of BADCOMMAND, PARAMPARSINGERROR, WRONGPARAMCOUNT,
		 * PERMISSIONDENIED or any int value a callback could return.
		 */
		int32 run(char* name, char* param_string, void* context);

		/**
		 * Execute a script.
		 * This method executes a command script contained into a buffer.
		 * The script can contain empty lines or # comments. They are
		 * ignored.
		 * @param script null terminated string.
		 * @param size size of the string.
		 * @param context an opaque pointer to the execution context of
		 * the commands.
		 * @return one of BADCOMMAND, PARAMPARSINGERROR, WRONGPARAMCOUNT,
		 * PERMISSIONDENIED or any int value a callback could return.
		 */
		int32 runScript(char* script, uint32 size, void* context);

		/**
		 * Execute command in the specified file.
		 * This method executes a file.
		 * @param filename the file containing the commands.
		 * @param linesz maximum line size in the file.
		 * @param context execution context for the interp.
		 * @return SUCCESS or FAILURE if an error occurs.
		 */
		int32 runFile(char* filename, uint32 linesz, void* context);
	protected:
		/**
		 * Redefine this method to handle custom command return codes.
		 * This method is invoked by runFile() and is used to handle
		 * return codes.
		 * @param cmd the executed command.
		 * @param curline current line in the file.
		 * @param ret value returned by current callback.
		 * @param context execution context for this interp.
		 */
		virtual void usrHandler(char* cmd, uint32 curline, int32 ret,
			void* context);

		/**
		 * Redefine this method to handle system return codes such as
		 * BADCOMMAND, PARAMPARSINGERROR and so on.
		 * This method is invoked by runFile() and is used to handle
		 * return codes.
		 * @param cmd the executed command.
		 * @param curline current line in the file.
		 * @param ret value returned by current callback.
		 * @param context execution context for this interp.
		 */
		virtual void sysHandler(char* cmd, uint32 curline, int32 ret,
			void* context);
	private:
		/* copy constructor not avilable */
		Interp(Interp&)
		{
		}

		/* enable params count check */
		bool m_bEnableCheck;
	};
}

#endif
