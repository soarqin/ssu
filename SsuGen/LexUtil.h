/*
 * Copyright (c) 2011, Soar Qin<soarchin@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LEXUTIL_H_
#define _LEXUTIL_H_

#include "Parser.h"
#include <cstdlib>
#include <cstdarg>

static void errorMsg(SSUStruct * st, const char * str, ...)
{
	/* Guess we need no more than 256 bytes. */
	int size = 256;

	char *p = new(std::nothrow) char[size];
	if (p == NULL)
		return;

	while (1) {
		va_list ap;
		va_start(ap, str);
		int n = vsnprintf(p, size, str, ap);
		va_end(ap);
		if (n > -1 && n < size)
			break;
		if (n > -1)    /* glibc 2.1 */
			size = n+1; /* precisely what is needed */
		else           /* glibc 2.0 */
			size *= 2;  /* twice the old size */
		delete[] p;
		if ((p = new(std::nothrow) char[size]) == NULL) {
			return;
		}
	}
	fprintf(stderr, "[ERR] %s:%d:%d  %s", st->fileName.back().c_str(), st->row.back(), st->col.back(), p);
	delete[] p;
	fprintf(stderr, "\n");
}

static unsigned int str_to_uint(SSUStruct * st, const char * str)
{
	char * endptr = NULL;
	unsigned int result;
	if(str[0] == '0')
	{
		if(str[1] == 'x' || str[1] == 'X')
			result = (unsigned int)strtoul(str, &endptr, 16);
		else
			result = (unsigned int)strtoul(str, &endptr, 8);
	}
	else
		result = (unsigned int)strtoul(str, &endptr, 10);
	if(endptr != NULL && *endptr != 0)
	{
		errorMsg(st, "'%s' Wrong number format!", str);
		exit(0);
	}
	return result;
}

static void importFile(SSUStruct * pss, const char * fname)
{
	if(fname[0] != '/' && fname[0] != '\\')
	{
		const std::string& fileName = *(pss->fileName.end() - 1);
		std::string::const_iterator it = fileName.end();
		while(it != fileName.begin())
		{
			char c = *(--it);
			if(c == '/' || c == '\\')
			{
				++ it;
				std::string newname(fileName.begin(), it);
				newname += fname;
				parse(newname.c_str(), *pss);
				printf_debug("import %s\n", newname.c_str());
				return;
			}
		}
	}
	printf_debug("import %s\n");
	parse(fname, *pss);
}

static void reset(SSUStruct * pss)
{
	pss->reset();
}

static void setPackage(SSUStruct * st)
{
	st->packageName = st->name;
}

static void setOption(SSUStruct * st)
{
	st->options[st->tname] = st->name;
}

static void addEnum(SSUStruct * st)
{
	EnumDef * ed = new EnumDef;
	if(st->currentStruct == NULL)
	{
		if(st->enums.find(st->name) != st->enums.end())
		{
			fprintf(stderr, "Repeated enum name! %s", st->name);
			exit(0);
		}
	}
	else
	{
		if(st->currentStruct->enums.find(st->name) != st->currentStruct->enums.end())
		{
			errorMsg(st, "'%s' Repeated enum name!", st->name);
			exit(0);
		}
	}
	ed->name = st->name;
	ed->comment = st->comment;
	st->comment[0] = 0;
	if(st->currentStruct == NULL)
	{
		st->enums[ed->name] = ed;
		st->enumList.push_back(ed);
	}
	else
	{
		st->currentStruct->enums[ed->name] = ed;
		st->currentStruct->enumList.push_back(ed);
	}
	st->currentEnum = ed;
}

static void endEnum(SSUStruct * st)
{
	st->currentEnum = NULL;
	st->comment[0] = 0;
}

static void addEnumVal(SSUStruct * st)
{
	if(st->currentEnum == NULL)
	{
		errorMsg(st, "Wrong enum defines!");
		exit(0);
	}
	if(st->currentEnum->vals.find(st->order) != st->currentEnum->vals.end())
	{
		errorMsg(st, "'%d' Repeated enum val order!", st->order);
		exit(0);
	}
	if(st->currentEnum->valMap.find(st->name) != st->currentEnum->valMap.end())
	{
		errorMsg(st, "'%s' Repeated enum val name!", st->name);
		exit(0);
	}
	EnumVal * ev = new EnumVal;
	ev->name = st->name;
	ev->val = st->order;
	ev->comment = st->comment;
	st->comment[0] = 0;
	st->currentEnum->vals[ev->val] = ev;
	st->currentEnum->valMap[ev->name] = ev;
}

static void addStruct(SSUStruct * st)
{
	printf_debug("Add Struct\n");
	bool repeat;
	if(st->currentStruct != NULL)
	{
		repeat = st->currentStruct->structs.find(st->name) != st->currentStruct->structs.end();
	}
	else
	{
		repeat = st->structs.find(st->name) != st->structs.end();
	}
	if(repeat)
	{
		errorMsg(st, "'%s' Repeated struct name!", st->name);
		exit(0);
	}
	StructDef * sd = new StructDef;
	sd->id = st->order;
	sd->name = st->name;
	sd->parent = st->currentStruct;
	sd->comment = st->comment;
	st->comment[0] = 0;
	if(st->currentStruct == NULL)
	{
		st->structs[sd->name] = sd;
		st->structList.push_back(sd);
	}
	else
	{
		st->currentStruct->structs[sd->name] = sd;
		st->currentStruct->structList.push_back(sd);
	}
	st->currentStruct = sd;
}

static void endStruct(SSUStruct * st)
{
	printf_debug("End Struct\n");
	if(st->currentStruct == NULL)
	{
		errorMsg(st, "Wrong struct end!");
		exit(0);
	}
	st->currentStruct = st->currentStruct->parent;
	st->comment[0] = 0;
}

static void appendField(SSUStruct * st)
{
	printf_debug("Append field\n");
	if(st->currentStruct == NULL)
	{
		errorMsg(st, "Wrong field!");
		exit(0);
	}
	if(st->order == 0)
	{
		if(st->currentStruct->fields.empty())
			st->order = 1;
		else
		{
			auto it = st->currentStruct->fields.end();
			-- it;
			st->order = it->second->order + 1;
		}
	}
	else if(st->currentStruct->fields.find(st->order) != st->currentStruct->fields.end())
	{
		errorMsg(st, "'%d' Repeated order number!", st->order);
		exit(0);
	}
	if(st->currentStruct->fieldMap.find(st->name) != st->currentStruct->fieldMap.end())
	{
		errorMsg(st, "'%s' Repeated field name !", st->name);
		exit(0);
	}
	if(st->type == TYPE_STRUCT)
	{
		bool found = false;
		if(st->currentStruct != NULL)
		{
			StructDef * sd = st->currentStruct;
			while(sd != NULL && !found)
			{
				if(sd->structs.find(st->tname) != sd->structs.end())
					found = true;
				else if(sd->enums.find(st->tname) != sd->enums.end())
				{
					st->type = TYPE_ENUM;
					found = true;
				}
				sd = sd->parent;
			}
		}
		if(!found)
		{
			if(st->structs.find(st->tname) != st->structs.end())
				found = true;
			else if(st->enums.find(st->tname) != st->enums.end())
			{
				st->type = TYPE_ENUM;
				found = true;
			}
		}
		if(!found)
		{
			errorMsg(st, "'%s' Wrong field type!", st->tname);
			exit(0);
		}
	}
	FieldDef * fd = new FieldDef;
	fd->order = st->order;
	fd->constraint = st->constraint;
	fd->type = st->type;
	fd->tname = st->tname;
	fd->name = st->name;
	fd->defVal = st->defVal;
	fd->comment = st->comment;
	st->comment[0] = 0;
	st->currentStruct->fields[st->order] = fd;
	st->currentStruct->fieldMap[st->name] = fd;
}

void onSyntaxError(SSUStruct * st)
{
	errorMsg(st, "'%s' Syntax error!", st->word.c_str());
	exit(0);
}

void onStackOverflow(SSUStruct * st)
{
	errorMsg(st, "'%s' Stack overflow!", st->word.c_str());
	exit(0);
}

#endif
