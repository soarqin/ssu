#include <cstdio>
#include <cassert>
#include <cstring>
#include <map>
#include <string>

#ifdef NDEBUG
#define printf_debug(fmt, ...)
#else
#define printf_debug(fmt, ...) fprintf(stdout, "[DEBUG] " fmt, ##__VA_ARGS__)
#endif

enum FieldType
{
	TYPE_INT,
	TYPE_UINT,
	TYPE_INT64,
	TYPE_UINT64,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_STRING,
	TYPE_BOOL,
	TYPE_VECTOR,
	TYPE_CUSTOM
};

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
	std::map<std::string, EnumVal *> vals;
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
	std::map<std::string, struct EnumDef *> enums;
	std::map<unsigned int, struct FieldDef *> fields;
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
	std::string packageName;
	std::map<std::string, std::string> options;
	std::map<std::string, StructDef *> structs;
	std::map<std::string, EnumDef *> enums;
	StructDef * currentStruct;
	EnumDef * currentEnum;
	int order, constraint, type;
	const char * tname;
	const char * name;
	const char * defVal;
	char comment[16384];
};

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
	ed->name = st->name;
	ed->comment = st->comment;
	st->comment[0] = 0;
	if(st->currentStruct == NULL)
		st->enums[ed->name] = ed;
	else
		st->currentStruct->enums[ed->name] = ed;
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
	EnumVal * ev = new EnumVal;
	ev->name = st->name;
	ev->val = st->order;
	ev->comment = st->comment;
	st->comment[0] = 0;
	st->currentEnum->vals[st->name] = ev;
}

static void addStruct(SSUStruct * st)
{
	printf_debug("Add Struct\n");
	StructDef * sd = new StructDef;
	sd->id = st->order;
	sd->name = st->name;
	sd->parent = st->currentStruct;
	sd->comment = st->comment;
	st->comment[0] = 0;
	if(st->currentStruct == NULL)
		st->structs[sd->name] = sd;
	else
		st->currentStruct->structs[sd->name] = sd;
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
	if(st->currentStruct->fields.find(st->order) != st->currentStruct->fields.end())
	{
		fprintf(stderr, "Repeated order number!\n");
		exit(0);
	}
	if(st->type == TYPE_CUSTOM)
	{
		bool found = false;
		if(st->currentStruct != NULL)
			found = st->currentStruct->structs.find(st->tname) != st->currentStruct->structs.end() || st->currentStruct->enums.find(st->tname) != st->currentStruct->enums.end();
		if(!found)
			found = st->structs.find(st->tname) != st->structs.end() || st->enums.find(st->tname) != st->enums.end();
		if(!found)
		{
			fprintf(stderr, "Wrong custom field type!\n");
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
}

#include "ssu.h"
#include "ssu.c"

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
	{"uint", TK_UINTEGER},
	{"int32", TK_INTEGER},
	{"uint32", TK_UINTEGER},
	{"int64", TK_INTEGER64},
	{"uint64", TK_UINTEGER64},
	{"float", TK_FLOAT},
	{"double", TK_DOUBLE},
	{"string", TK_STRING},
	{"bool", TK_BOOL},
	{"bytes", TK_VECTOR},
	{"default", TK_DEFAULT},
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

int main(int, char *[])
{
	void * parser = ssuParserAlloc(malloc);
	FILE * f = fopen("test.s", "rt");
	SSUStruct ssus;
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

	for(auto it = ssus.structs.begin(); it != ssus.structs.end(); ++ it)
	{
		printf("Struct %s:\n", it->second->name.c_str());
		for(auto it2 = it->second->fields.begin(); it2 != it->second->fields.end(); ++ it2)
		{
			printf("  Field [%d,%d] %s: %d    comment: %s\n", it2->second->order, it2->second->constraint, it2->second->name.c_str(), it2->second->type, it2->second->comment.c_str());
		}
	}
	return 0;
}
