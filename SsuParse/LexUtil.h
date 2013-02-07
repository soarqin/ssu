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

#ifdef _DEBUG
#define DEBUG_PRINTF
#endif

#ifdef DEBUG_PRINTF
#define printf_debug(fmt, ...) fprintf(stderr, "[DEBUG] " fmt, ##__VA_ARGS__)
#else
#define printf_debug(fmt, ...)
#endif

struct SSUHelper
{
	SSUHelper(): currentStruct(NULL), currentEnum(NULL) {reset(); comment[0] = 0;}
	void reset()
	{
		order = 0;
		constraint = 0;
		type = 0;
		tname = "";
		name = "";
		defVal = "";
	}
	std::vector<std::string> fileName;
	std::vector<int> row, col;
	std::string word;
	StructDef * currentStruct;
	EnumDef * currentEnum;
	int order, constraint, type;
	const char * tname;
	const char * name;
	const char * defVal;
	char comment[16384];
};

struct SSUParseStruct
{
	SSUStruct ss;
	SSUHelper ssh;
};

static void errorMsg(SSUParseStruct * st, const char * str, ...)
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
	fprintf(stderr, "[ERR] %s:%d:%d  %s", st->ssh.fileName.back().c_str(), st->ssh.row.back(), st->ssh.col.back(), p);
	delete[] p;
	fprintf(stderr, "\n");
}

static unsigned int str_to_uint(SSUParseStruct * st, const char * str)
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

static void importFile(SSUParseStruct * pss, const char * fname)
{
	if(fname[0] != '/' && fname[0] != '\\')
	{
		const std::string& fileName = *(pss->ssh.fileName.end() - 1);
		std::string::const_iterator it = fileName.end();
		while(it != fileName.begin())
		{
			char c = *(--it);
			if(c == '/' || c == '\\')
			{
				++ it;
				std::string newname(fileName.begin(), it);
				newname += fname;
				realParse(newname.c_str(), pss);
				printf_debug("import %s\n", newname.c_str());
				return;
			}
		}
	}
	printf_debug("import %s\n", fname);
	realParse(fname, pss);
}

static void addEnum(SSUParseStruct * st)
{
	EnumDef * ed = new EnumDef;
	if(st->ssh.currentStruct == NULL)
	{
		if(st->ss.enums.find(st->ssh.name) != st->ss.enums.end())
		{
			fprintf(stderr, "Repeated enum name! %s", st->ssh.name);
			exit(0);
		}
	}
	else
	{
		if(st->ssh.currentStruct->enums.find(st->ssh.name) != st->ssh.currentStruct->enums.end())
		{
			errorMsg(st, "'%s' Repeated enum name!", st->ssh.name);
			exit(0);
		}
	}
	ed->name = st->ssh.name;
	ed->comment = st->ssh.comment;
	st->ssh.comment[0] = 0;
	if(st->ssh.currentStruct == NULL)
	{
		st->ss.enums[ed->name] = ed;
		st->ss.enumList.push_back(ed);
	}
	else
	{
		st->ssh.currentStruct->enums[ed->name] = ed;
		st->ssh.currentStruct->enumList.push_back(ed);
	}
	st->ssh.currentEnum = ed;
}

static void endEnum(SSUParseStruct * st)
{
	st->ssh.currentEnum = NULL;
	st->ssh.comment[0] = 0;
}

static void addEnumVal(SSUParseStruct * st)
{
	if(st->ssh.currentEnum == NULL)
	{
		errorMsg(st, "Wrong enum defines!");
		exit(0);
	}
	if(st->ssh.currentEnum->vals.find(st->ssh.order) != st->ssh.currentEnum->vals.end())
	{
		errorMsg(st, "'%d' Repeated enum val order!", st->ssh.order);
		exit(0);
	}
	if(st->ssh.currentEnum->valMap.find(st->ssh.name) != st->ssh.currentEnum->valMap.end())
	{
		errorMsg(st, "'%s' Repeated enum val name!", st->ssh.name);
		exit(0);
	}
	EnumVal * ev = new EnumVal;
	ev->name = st->ssh.name;
	ev->val = st->ssh.order;
	ev->comment = st->ssh.comment;
	st->ssh.comment[0] = 0;
	st->ssh.currentEnum->vals[ev->val] = ev;
	st->ssh.currentEnum->valMap[ev->name] = ev;
}

static void addStruct(SSUParseStruct * st)
{
	printf_debug("Add Struct\n");
	bool repeat;
	if(st->ssh.currentStruct != NULL)
	{
		repeat = st->ssh.currentStruct->structs.find(st->ssh.name) != st->ssh.currentStruct->structs.end();
	}
	else
	{
		repeat = st->ss.structs.find(st->ssh.name) != st->ss.structs.end();
	}
	if(repeat)
	{
		errorMsg(st, "'%s' Repeated struct name!", st->ssh.name);
		exit(0);
	}
	StructDef * sd = new StructDef;
	sd->id = st->ssh.order;
	sd->name = st->ssh.name;
	sd->parent = st->ssh.currentStruct;
	sd->comment = st->ssh.comment;
	st->ssh.comment[0] = 0;
	if(st->ssh.currentStruct == NULL)
	{
		st->ss.structs[sd->name] = sd;
		st->ss.structList.push_back(sd);
	}
	else
	{
		st->ssh.currentStruct->structs[sd->name] = sd;
		st->ssh.currentStruct->structList.push_back(sd);
	}
	st->ssh.currentStruct = sd;
}

static void endStruct(SSUParseStruct * st)
{
	printf_debug("End Struct\n");
	if(st->ssh.currentStruct == NULL)
	{
		errorMsg(st, "Wrong struct end!");
		exit(0);
	}
	st->ssh.currentStruct = st->ssh.currentStruct->parent;
	st->ssh.comment[0] = 0;
}

static void appendField(SSUParseStruct * st)
{
	printf_debug("Append field\n");
	if(st->ssh.currentStruct == NULL)
	{
		errorMsg(st, "Wrong field!");
		exit(0);
	}
	if(st->ssh.order == 0)
	{
		if(st->ssh.currentStruct->fields.empty())
			st->ssh.order = 1;
		else
		{
			auto it = st->ssh.currentStruct->fields.end();
			-- it;
			st->ssh.order = it->second->order + 1;
		}
	}
	else if(st->ssh.currentStruct->fields.find(st->ssh.order) != st->ssh.currentStruct->fields.end())
	{
		errorMsg(st, "'%d' Repeated order number!", st->ssh.order);
		exit(0);
	}
	if(st->ssh.currentStruct->fieldMap.find(st->ssh.name) != st->ssh.currentStruct->fieldMap.end())
	{
		errorMsg(st, "'%s' Repeated field name !", st->ssh.name);
		exit(0);
	}
	if(st->ssh.type == TYPE_STRUCT)
	{
		bool found = false;
		if(st->ssh.currentStruct != NULL)
		{
			StructDef * sd = st->ssh.currentStruct;
			while(sd != NULL && !found)
			{
				if(sd->structs.find(st->ssh.tname) != sd->structs.end())
					found = true;
				else if(sd->enums.find(st->ssh.tname) != sd->enums.end())
				{
					st->ssh.type = TYPE_ENUM;
					found = true;
				}
				sd = sd->parent;
			}
		}
		if(!found)
		{
			if(st->ss.structs.find(st->ssh.tname) != st->ss.structs.end())
				found = true;
			else if(st->ss.enums.find(st->ssh.tname) != st->ss.enums.end())
			{
				st->ssh.type = TYPE_ENUM;
				found = true;
			}
		}
		if(!found)
		{
			errorMsg(st, "'%s' Wrong field type!", st->ssh.tname);
			exit(0);
		}
	}
	FieldDef * fd = new FieldDef;
	fd->order = st->ssh.order;
	fd->constraint = st->ssh.constraint;
	fd->type = st->ssh.type;
	fd->tname = st->ssh.tname;
	fd->name = st->ssh.name;
	fd->defVal = st->ssh.defVal;
	fd->comment = st->ssh.comment;
	st->ssh.comment[0] = 0;
	st->ssh.currentStruct->fields[st->ssh.order] = fd;
	st->ssh.currentStruct->fieldMap[st->ssh.name] = fd;
}

void onSyntaxError(SSUParseStruct * st)
{
	errorMsg(st, "'%s' Syntax error!", st->ssh.word.c_str());
	exit(0);
}

void onStackOverflow(SSUParseStruct * st)
{
	errorMsg(st, "'%s' Stack overflow!", st->ssh.word.c_str());
	exit(0);
}

#endif
