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

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#include "utils.hpp"
#include "interp.hpp"

namespace uStreamLib {
	/*
	 * Implementation of Params class.
	 */

	Params::Params(void)
		: Object(UOSUTIL_RTTI_PARAMS), m_strParam(NULL), m_uParamsFound(0),
		m_uMaxParamsNo(0), m_uMaxParamSize(0)
	{
		// nothing to do
	}

	Params::~Params(void)
	{
		for (uint32 i = 0; i < m_uParamsFound; i++)
			if (m_strParam[i])
				::free(m_strParam[i]);

		free(m_strParam);
	}

	int32 Params::init(uint32 max_param_no, uint32 max_param_sz)
	{
		int32 ret = 0;

		// initialize command name buffer
		ret = m_dbCmdName.init(32);
		if (ret == FAILURE)
			return FAILURE;

		// zeroize command name buffer
		m_dbCmdName.set(0);

		// allocate memory for parameters
		m_strParam = (char * *) malloc(max_param_no * sizeof(char *));
		if (!m_strParam)
			return FAILURE;

		for (uint32 i = 0; i < max_param_no; i++) {
			m_strParam[i] = (char *) malloc(max_param_sz);
			if (!m_strParam[i])
				return FAILURE;

			memset(m_strParam[i], 0, max_param_sz);
		}

		m_uParamsFound = 0;
		m_uMaxParamsNo = max_param_no;
		m_uMaxParamSize = max_param_sz;

		// ok
		setOk(true);
		return SUCCESS;
	}

	int32 Params::add(char* string)
	{
		uint32 sz = 0;

		if (m_uParamsFound >= m_uMaxParamsNo)
			return FAILURE;

		sz = (uint32) strlen(string) + 1;
		strncpy(m_strParam[m_uParamsFound], string,
			sz >= m_uMaxParamSize ? m_uMaxParamSize : sz);
		m_strParam[m_uParamsFound][sz - 1] = 0;

		m_uParamsFound++;
		return SUCCESS;
	}

	/*
	 * Implementation of base Command class.
	 */

	Command::Command(void)
		: m_voidCtx(NULL), m_bEnabled(false), m_uMinParamsCount(0),
		m_uMaxParamsCount(0)
	{
		setClassID(UOSUTIL_RTTI_COMMAND);
	}

	Command::~Command(void)
	{
		// nothing to do
	}

	int32 Command::init(char* name, // command name
	uint32 min_pc, // minimum acceptable parameters count
	uint32 max_pc, // maximum acceptable parameters count
	void* context   // generic user data pointer
	)
	{
		int32 ret = 0;

		// initialize parent
		ret = CallBack::init(name);
		if (ret == FAILURE)
			return FAILURE;

		// initialize members
		m_bEnabled = true;
		m_uMinParamsCount = min_pc;
		m_uMaxParamsCount = max_pc;
		m_voidCtx = context;

		// initialize string buffers
		ret = m_dbSyntax.init(32);
		if (ret == FAILURE)
			return FAILURE;

		ret = m_dbDesc.init(32);
		if (ret == FAILURE)
			return FAILURE;

		ret = m_dbWork.init(32);
		if (ret == FAILURE)
			return FAILURE;

		// initialize buffers
		m_dbSyntax.set(0);
		m_dbSyntax.setCount(0);

		m_dbDesc.set(0);
		m_dbDesc.setCount(0);

		m_dbWork.set(0);
		m_dbWork.setCount(0);

		// ok
		setOk(true);
		return SUCCESS;
	}

	int32 Command::makeParams(Params* p, char* instr_string)
	{
		int32 i, j = 0, k = 0;
		int32 token_found = 0;

		int32 strsz = (int32) strlen(instr_string) + 1;
		int32 ret = 0;

		char* in = instr_string;
		char* token = NULL, * temp = NULL;
		char* first = NULL, * ttemp = NULL;

		m_dbWork.setCount(0);

		ret = m_dbWork.realloc(strsz);
		if (ret == FAILURE)
			return FAILURE;

		temp = m_dbWork.toString();
		first = temp;

		for (i = 0; i < strsz; i++, in++) {
			*temp = *in;
			temp++;

			if (*in == '\"') {
				temp = first;

				while (++i < strsz && *(++in) != '\"') {
					*temp = *in; temp++;
				}

				*temp = '\0';
				token_found = 1;
				temp = first;

				in++; i++;
			} else if (*in == ' ' || *in == '\t' || *in == '\0') {
				*(temp - 1) = '\0';
				temp = first;
				token_found = 1;
			}

			if (token_found) {
				token = temp;

				k = (int32) strlen(token);
				while (k && isspace(toascii(*token))) {
					token++; k--;
				}

				k = (int32) strlen(token);
				ttemp = &token[k];
				while (k &&
					(isspace(toascii(*ttemp)) || iscntrl(toascii(*ttemp)))) {
					*ttemp = 0; ttemp--;
				}

				if (p->add(token) == FAILURE)
					return FAILURE;

				token_found = 0;
			}
		}

		return SUCCESS;
	}

	/*
	 * Implementation of Interp class.
	 */

	Interp::Interp(void)
		: m_bEnableCheck(true)
	{
		// nothing to do
	}

	Interp::~Interp(void)
	{
		deleteCommands();
	}

	int32 Interp::init(uint32 table_size, uint32 max_param_no,
		uint32 max_param_sz)
	{
		int32 ret = 0;

		// initialize shared hash
		ret = SharedHash::init(table_size);
		if (ret == FAILURE)
			return FAILURE;

		// initialize params container
		ret = Params::init(max_param_no, max_param_sz);
		if (ret == FAILURE)
			return FAILURE;

		// ok
		Params::setOk(true);
		return SUCCESS;
	}

	void Interp::sysHandler(char* token, uint32 curline, int32 ret,
		void* context)
	{
		fprintf(stderr, "System Error: %0X on line %d\n", ret, curline);
		fprintf(stderr, " Token: \"%s\"\n", token);
	}

	void Interp::usrHandler(char* token, uint32 curline, int32 ret,
		void* context)
	{
		UOSUTIL_DOUT(("User Return: %0X on line %d\n", ret, curline));
		UOSUTIL_DOUT((" Token: \"%s\"\n", token));
	}

	void Interp::deleteCommands(void)
	{
		Enumeration* e = keys();
		while (e->hasMoreElements()) {
			char* com_name = (char*) e->nextElement();
			Command* c = (Command*) get(com_name);
			if (c)
				delete c;
		}
	}

	int32 Interp::addCommand(Command* c)
	{
		c->setEnabled(true);
		return pput(c->getName(), (char *) c);
	}

	int32 Interp::delCommand(char* name)
	{
		return del(name);
	}

	Command* Interp::getCommand(char* name)
	{
		return (Command *) get(name);
	}

	int32 Interp::enableCommand(char* name, bool enable)
	{
		Command* c = (Command*) get(name);
		if (!c)
			return FAILURE;

		c->setEnabled(enable);
		return SUCCESS;
	}

	int32 Interp::run(char* name, char* param_string, void* context)
	{
		Command* c = NULL;
		int32 ret = 0;

		uint32 out = 0;

		/* reset parameters descriptor */
		reset();

		/* set command name */
		setCommandName(name);

		/* search for the instruction */
		c = (Command *) get(name);
		if (!c)
			return BADCOMMAND;

		/* check if there are parameters */
		if (param_string) {
			/* create parameters */
			ret = c->makeParams(this, param_string);
			if (ret == FAILURE)
				return PARAMPARSINGERROR;
		}

		/* check parameters count */
		if (m_bEnableCheck) {
			if (!c->checkParamsCount(Params::getCount()))
				return WRONGPARAMCOUNT;
		}

		/* execute action */
		if (c->isEnabled())
			out = c->perform(this, context);
		else
			return PERMISSIONDENIED;

		/* return callback output */
		return out;
	}

	int32 Interp::runScript(char* script, uint32 size, void* context)
	{
		char* buf = script, * line = NULL;
		char* token = NULL, * param_string = NULL;
		int32 out = 0, current_line = 0, ret = 0, k = 0;

		while (1) {
			line = Utils::getToken(&buf, CRLF, size, 2);
			if (!line)
				break;

			/* remove '\n' and blanks */
			out = (int32) strlen(line);
			while (--out && isspace(toascii(line[out])))
				line[out] = 0;

			/* remove blanks */
			k = (int32) strlen(line);
			while (k && isspace(toascii(*line))) {
				k--; line++;
			}

			/* remove empty lines and comments */
			if (line[0] == '#' || line[0] == '\0') {
				current_line++; continue;
			}

			/* execute instruction */
			param_string = line;
			token = Utils::getToken(&param_string, " \t");
			if (token)
				ret = run(token, param_string, context);
			else {
				current_line++; continue;
			}

			/* get return value */
			switch (ret) {
			case Interp::BADCOMMAND:
			case Interp::PARAMPARSINGERROR:
			case Interp::WRONGPARAMCOUNT:
			case Interp::PERMISSIONDENIED:
				sysHandler(token, current_line, ret, context);
				return FAILURE;
			default:
				usrHandler(token, current_line, ret, context);
			}

			current_line++;
		}

		return SUCCESS;
	}

	int32 Interp::runFile(char* filename, uint32 linesz, void* context)
	{
		DataBuf dbLine;
		FILE* fin = NULL;

		char* param_string = NULL;
		char* token = NULL;
		char* line = NULL;

		uint32 current_line = 1;
		int32 ret = 0, out = 0, k = 0;

		ret = dbLine.init(linesz);
		if (ret == FAILURE)
			return FAILURE;

		dbLine.set(0);
		dbLine.setCount(0);
		dbLine.setLimit(2 * 1024 * 1024);

		line = dbLine.toString();

		fin = fopen(filename, "r");
		if (!fin)
			return FAILURE;

		while (1) {
			/* get a line */
			out = Utils::getDelim(line, linesz, '\n', fin);
			if (out < 0)
				break;

			/* remove '\n' and blanks */
			while (--out && isspace(toascii(line[out])))
				line[out] = 0;

			/* remove blanks */
			k = (int32) strlen(line);
			while (k && isspace(toascii(*line))) {
				k--; line++;
			}

			/* remove empty lines and comments */
			if (line[0] == '#' || line[0] == '\0') {
				current_line++; continue;
			}

			/* execute instruction */
			param_string = line;
			token = Utils::getToken(&param_string, " \t");
			if (token) {
				ret = run(token, param_string, context);
				switch (ret) {
				case BADCOMMAND:
				case PARAMPARSINGERROR:
				case WRONGPARAMCOUNT:
				case PERMISSIONDENIED:
					sysHandler(token, current_line, ret, context);
					return FAILURE;
				default:
					usrHandler(token, current_line, ret, context);
				}

				current_line++;
			} else {
				current_line++;
				continue;
			}
		}

		fclose(fin);
		return SUCCESS;
	}
}
