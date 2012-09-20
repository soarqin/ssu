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

#include "LexUtil.h"
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
	{"import", TK_IMPORT},
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

inline void push(void * parser, SSUStruct * ssus)
{
	TokenAssign * assign;
	for(assign = tokenAssigns; assign->token != NULL; ++ assign)
	{
		if(strcmp(assign->token, ssus->word.c_str()) == 0)
		{
			printf_debug("%s %d\n", ssus->word.c_str(), assign->id);
			ssuParser(parser, assign->id, strdup(ssus->word.c_str()), ssus);
			return;
		}
	}
	printf_debug("%s\n", ssus->word.c_str());
	ssuParser(parser, TK_CUSTOM, strdup(ssus->word.c_str()), ssus);
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
		ssus.col.back() = sstart - s + 1; \
		ssus.word.assign(sstart, scurrent); \
		push(parser, &ssus); \
	}


void parse(const char * filename, SSUStruct& ssus)
{
	void * parser = ssuParserAlloc(malloc);
	FILE * f = fopen(filename, "rt");
	ssus.fileName.push_back(filename);
	ssus.row.push_back(0);
	ssus.col.push_back(0);
	int lineNo = 0;
	while(!feof(f))
	{
		char s[4096];
		++ lineNo;
		if(fgets(s, 4096, f) == NULL)
			continue;
		++ ssus.row.back();
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
			fprintf(stderr, "[%s] %d:1  Unpaired quotes!", filename, lineNo);
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
					PUSH_LASTTOKEN;
				}
				nextLine = true;
				break;
			case 8:
				{
					PUSH_LASTTOKEN;
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
					PUSH_LASTTOKEN;
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
					ssus.col.back() = scurrent - s + 1;
					ssus.word = *scurrent;
					push(parser, &ssus);
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
	ssus.fileName.erase(ssus.fileName.end() - 1);
	ssus.row.erase(ssus.row.end() - 1);
	ssus.col.erase(ssus.col.end() - 1);
}
