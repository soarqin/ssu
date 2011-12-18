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

#include "Parser.h"
#include <cstdlib>

static unsigned int str_to_uint(const char * str)
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
		fprintf(stderr, "Wrong number format: %s!\n", str);
		exit(0);
	}
	return result;
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
			fprintf(stderr, "Repeated enum name! %s\n", st->name);
			exit(0);
		}
	}
	else
	{
		if(st->currentStruct->enums.find(st->name) != st->currentStruct->enums.end())
		{
			fprintf(stderr, "Repeated enum name! %s\n", st->name);
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
		fprintf(stderr, "Wrong enum defines!\n");
		exit(0);
	}
	if(st->currentEnum->vals.find(st->order) != st->currentEnum->vals.end())
	{
		fprintf(stderr, "Repeated enum val order! %d\n", st->order);
		exit(0);
	}
	if(st->currentEnum->valMap.find(st->name) != st->currentEnum->valMap.end())
	{
		fprintf(stderr, "Repeated enum val name! %s\n", st->name);
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
		fprintf(stderr, "Repeated struct name %s!\n", st->name);
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
		fprintf(stderr, "Wrong struct end!\n");
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
		fprintf(stderr, "Wrong field!\n");
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
		fprintf(stderr, "Repeated order number %d!\n", st->order);
		exit(0);
	}
	if(st->currentStruct->fieldMap.find(st->name) != st->currentStruct->fieldMap.end())
	{
		fprintf(stderr, "Repeated field name %s!\n", st->name);
		exit(0);
	}
	if(st->type == TYPE_STRUCT)
	{
		bool found = false;
		if(st->currentStruct != NULL)
		{
			if(st->currentStruct->structs.find(st->tname) != st->currentStruct->structs.end())
				found = true;
			else if(st->currentStruct->enums.find(st->tname) != st->currentStruct->enums.end())
			{
				st->type = TYPE_ENUM;
				found = true;
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
			fprintf(stderr, "Wrong field type %s!\n", st->tname);
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

#include "SsuLex.h"
#include "SsuLex.c"

struct TokenAssign
{
	const char * token;
	int id;
} tokenAssigns[] =
{
	{"struct", TK_STRUCT},
	{"message", TK_STRUCT},
	{"class", TK_STRUCT},
	{"enum", TK_ENUM},
	{"option", TK_OPTION},
	{"package", TK_PACKAGE},
	{"{", TK_LBRACE},
	{"}", TK_RBRACE},
	{"[", TK_LSBRACKET},
	{"]", TK_RSBRACKET},
	{",", TK_COMMA},
	{"=", TK_ASSIGN},
	{";", TK_DELIM},
	{"required", TK_REQUIRED},
	{"optional", TK_OPTIONAL},
	{"repeated", TK_REPEATED},
	{"ordermap", TK_ORDERMAP},
	{"int", TK_INTEGER},
	{"sint", TK_SINTEGER},
	{"uint", TK_UINTEGER},
	{"int32", TK_INTEGER},
	{"sint32", TK_SINTEGER},
	{"uint32", TK_UINTEGER},
	{"int64", TK_INTEGER64},
	{"sint64", TK_SINTEGER64},
	{"uint64", TK_UINTEGER64},
	{"float", TK_FLOAT},
	{"double", TK_DOUBLE},
	{"string", TK_STRING},
	{"bool", TK_BOOL},
	{"default", TK_DEFAULT},
	{"packed", TK_PACKED},
	{NULL, TK_CUSTOM},
};

inline void push(void * parser, SSUStruct * ssus, const char * token)
{
	TokenAssign * assign;
	for(assign = tokenAssigns; assign->token != NULL; ++ assign)
	{
		if(strcmp(assign->token, token) == 0)
		{
			printf_debug("%s %d\n", token, assign->id);
			ssuParser(parser, assign->id, strdup(token), ssus);
			return;
		}
	}
	printf_debug("%s\n", token);
	ssuParser(parser, TK_CUSTOM, strdup(token), ssus);
}

inline int typeFromChar(unsigned char v)
{
	const int cTable[256] = {
		//  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
		9, 0, 0, 0, 0, 0, 0, 0, 0, 8, 9, 0, 0, 9, 0, 0, // 0
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
		8, 0, 7, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1, 1, 0, // 2
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, // 3
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, // 5
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // B
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // C
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // D
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // E
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F
	};
	return cTable[v];
}

static char * extractComment(char * s, int& err)
{
	err = 0;
	char *p = s;
	while(*p != 0)
	{
		if(*p == '\'' || *p == '"')
		{
			p = strchr(p + 1, *p);
			if(p == NULL)
			{
				err = 1;
				return NULL;
			}
		}
		else if(*p == '/' && *(p + 1) == '/')
		{
			*p = 0;
			return p + 2;
		}
		++ p;
	}
	return NULL;
}

#define PUSH_LASTTOKEN \
	if((currentCharId == 0 || currentCharId == 1) && sstart != scurrent) \
	{ \
		std::string tmpStr(sstart, scurrent); \
		push(parser, &ssus, tmpStr.c_str()); \
	}


void parse(const char * filename, SSUStruct& ssus)
{
	void * parser = ssuParserAlloc(malloc);
	FILE * f = fopen(filename, "rt");

	while(!feof(f))
	{
		char s[4096];
		if(fgets(s, 4096, f) == NULL)
			continue;
		size_t len = strlen(s);
		if(len == 0)
			continue;
		int i = (int)len - 1;
		while(i >= 0 && typeFromChar(s[i]) == 9)
			-- i;
		if(i < 0)
			continue;
		s[i + 1] = 0;
		int err = 0;
		char * cmt = extractComment(s, err);
		if(err)
		{
			fprintf(stderr, "Error! unpaired quotes!");
			exit(0);
		}
		if(cmt != NULL)
			ssuParser(parser, TK_COMMENT, strdup(cmt), &ssus);
		char * sstart = s;
		char * scurrent = s;
		bool nextLine = false;
		int currentCharId = -1;
		while(!nextLine)
		{
			int charId = typeFromChar(*scurrent);
			switch(charId)
			{
			case 9:
				if(scurrent != sstart)
				{
					PUSH_LASTTOKEN
				}
				nextLine = true;
				break;
			case 8:
				{
					PUSH_LASTTOKEN
						do
						{
							++ scurrent;
						}
						while(typeFromChar(*scurrent) == 8);
						sstart = scurrent;
						currentCharId = -1;
						continue;
				}
			case 7:
				{
					PUSH_LASTTOKEN
						sstart = scurrent + 1;
					scurrent = strchr(sstart, *scurrent);
					std::string tmpStr2(sstart, scurrent);
					ssuParser(parser, TK_CUSTOM, strdup(tmpStr2.c_str()), &ssus);
					sstart = scurrent + 1;
				}
				break;
			case 1:
			case 0:
				if(charId != currentCharId)
				{
					PUSH_LASTTOKEN
						currentCharId = charId;
					sstart = scurrent;
				}
				if(charId == 0)
				{
					char vstr[2] = {*scurrent, 0};
					push(parser, &ssus, vstr);
					sstart = scurrent + 1;
				}
				break;
			}
			if(nextLine)
				break;
			++ scurrent;
		}
	}
	fclose(f);

	ssuParser(parser, 0, NULL, &ssus);
	ssuParserFree(parser, free);
}
