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

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdarg>
#include <map>
#include <string>
#include <vector>

#ifdef _DEBUG
#define DEBUG_PRINTF
#endif

#ifdef DEBUG_PRINTF
#define printf_debug(fmt, ...) fprintf(stderr, "[DEBUG] " fmt, ##__VA_ARGS__)
#else
#define printf_debug(fmt, ...)
#endif

enum FieldType
{
	TYPE_INT,
	TYPE_SINT,
	TYPE_UINT,
	TYPE_INT64,
	TYPE_SINT64,
	TYPE_UINT64,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_STRING,
	TYPE_BOOL,
	TYPE_ENUM,
	TYPE_STRUCT
};


struct FieldDef
{
	FieldDef(): order(0), constraint(0), type(0) {}
	int order;
	int constraint;
	int type;
	std::string tname;
	std::string name;
	std::string defVal;
	std::string comment;
};

struct EnumVal
{
	std::string name;
	int val;
	std::string comment;
};

struct EnumDef
{
	~EnumDef()
	{
		for(auto it = vals.begin(); it != vals.end(); ++ it)
		{
			delete it->second;
		}
		vals.clear();
	}
	std::string name;
	std::map<int, EnumVal *> vals;
	std::map<std::string, EnumVal *> valMap;
	std::string comment;
};

struct StructDef
{
	StructDef(): id(0), parent(NULL) {}
	~StructDef()
	{
		for(auto it = structs.begin(); it != structs.end(); ++ it)
		{
			delete it->second;
		}
		structs.clear();
		for(auto it = enums.begin(); it != enums.end(); ++ it)
		{
			delete it->second;
		}
		enums.clear();
		for(auto it = fields.begin(); it != fields.end(); ++ it)
		{
			delete it->second;
		}
		fields.clear();
	}
	unsigned int id;
	std::string name;
	StructDef * parent;
	std::map<std::string, struct StructDef *> structs;
	std::vector<struct StructDef *> structList;
	std::map<std::string, struct EnumDef *> enums;
	std::vector<struct EnumDef *> enumList;
	std::map<unsigned int, struct FieldDef *> fields;
	std::map<std::string, struct FieldDef *> fieldMap;
	std::string comment;
};

struct SSUStruct
{
	SSUStruct(): currentStruct(NULL), currentEnum(NULL) {reset(); comment[0] = 0;}
	~SSUStruct()
	{
		for(auto it = structs.begin(); it != structs.end(); ++ it)
		{
			delete it->second;
		}
		structs.clear();
		for(auto it = enums.begin(); it != enums.end(); ++ it)
		{
			delete it->second;
		}
		enums.clear();
	}
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
	std::string packageName;
	std::map<std::string, std::string> options;
	std::map<std::string, StructDef *> structs;
	std::vector<StructDef *> structList;
	std::map<std::string, EnumDef *> enums;
	std::vector<EnumDef *> enumList;
	StructDef * currentStruct;
	EnumDef * currentEnum;
	int order, constraint, type;
	const char * tname;
	const char * name;
	const char * defVal;
	char comment[16384];
};

extern void parse(const char * filename, SSUStruct&);
