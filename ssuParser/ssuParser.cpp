#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdarg>
#include <map>
#include <string>
#include <vector>

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

std::string capitalize(const std::string& oldName, bool capitalFirst = false)
{
	std::string result;
	bool needCapital = capitalFirst;
	auto it = oldName.begin();
	while(it != oldName.end() && (*it) == '_')
		++ it;
	for(; it != oldName.end(); ++ it)
	{
		if((*it) == '_')
			needCapital = true;
		else
		{
			if(needCapital)
			{
				if((*it) >= 'a' && (*it) <= 'z')
					result += (*it) - 32;
				else
					result += *it;
				needCapital = false;
			}
			else
				result += *it;
		}
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
	if(st->type == TYPE_CUSTOM)
	{
		bool found = false;
		if(st->currentStruct != NULL)
			found = st->currentStruct->structs.find(st->tname) != st->currentStruct->structs.end() || st->currentStruct->enums.find(st->tname) != st->currentStruct->enums.end();
		if(!found)
			found = st->structs.find(st->tname) != st->structs.end() || st->enums.find(st->tname) != st->enums.end();
		if(!found)
		{
			fprintf(stderr, "Wrong custom field type %s!\n", st->tname);
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

void process(SSUStruct&);

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

	process(ssus);
	return 0;
}

int indentSize = 2;

void fprintIndent(int indent, FILE * fp, const char * pattern, ...)
{
	va_list argp;
	va_start(argp, pattern);
	for(int i = 0; i < indent; ++ i)
		fputc(' ', fp);
	vfprintf(fp, pattern, argp);
	va_end(argp);
}

char * sprintIndent(int indent, char * str, const char * pattern, ...)
{
	va_list argp;
	memset(str, ' ', indent);
	str[indent] = 0;
	va_start(argp, pattern);
	vsprintf(str + indent, pattern, argp);
	va_end(argp);
	return str;
}

void printEnum(std::vector<EnumDef *>& ed, int indent)
{
	for(auto it = ed.begin(); it != ed.end(); ++ it)
	{
		fprintIndent(indent, stdout, "enum %s\n", (*it)->name.c_str());
		fprintIndent(indent, stdout, "{\n");
		indent += indentSize;
		for(auto it2 = (*it)->vals.begin(); it2 != (*it)->vals.end(); ++ it2)
		{
			fprintIndent(indent, stdout, "%s = %d,\n", it2->second->name, it2->second->val);
		}
		indent -= indentSize;
	}
}

void printField(std::string& pstr, std::string& rstr, std::string& cstr, std::string& dstr, bool useRef, int constraint, int order, const std::string& type, const std::string& name, const std::string& defVal, int indent)
{
	char tmpStr[1024];

	std::string lName = capitalize(name);
	std::string uName = capitalize(name, true);

	if(constraint == 1 || constraint == 2)
	{
		if(useRef)
			rstr += sprintIndent(indent, tmpStr, "ssu::ReferredObject<%s> _%s;\n\n", type.c_str(), name.c_str());
		else
			rstr += sprintIndent(indent, tmpStr, "%s _%s;\n\n", type.c_str(), name.c_str());

		if(constraint == 1)
		{
			if(useRef)
			{
				pstr += sprintIndent(indent, tmpStr, "inline const %s& %s() const { return _%s; }\n", type.c_str(), lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void set%s(const %s& val__) { _%s = val__; }\n", uName.c_str(), type.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline %s * mutable%s() { return &_%s; }\n", type.c_str(), uName.c_str(), name.c_str());
			}
			else
			{
				pstr += sprintIndent(indent, tmpStr, "inline %s %s() const { return _%s; }\n", type.c_str(), lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void set%s(%s val__) { _%s = val__; }\n", uName.c_str(), type.c_str(), name.c_str());
			}
		}
		else
		{
			if(useRef)
			{
				pstr += sprintIndent(indent, tmpStr, "inline const %s& %s() const { return _%s; }\n", type.c_str(), lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void set%s(const %s& val__) { _%s = val__; _isSetFlag[%d] |= 0x%02X; }\n", uName.c_str(), type.c_str(), name.c_str(), order / 32, 1 << (order % 32));
				pstr += sprintIndent(indent, tmpStr, "inline %s * mutable%s() { _isSetFlag[%d] |= 0x%02X; return _%s.getMutable(); }\n", type.c_str(), uName.c_str(), order / 32, 1 << (order % 32), name.c_str());
			}
			else
			{
				pstr += sprintIndent(indent, tmpStr, "inline %s %s() const { return _%s; }\n", type.c_str(), lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void set%s(%s val__) { _%s = val__; _isSetFlag[%d] |= 0x%02X; }\n", uName.c_str(), type.c_str(), name.c_str(), order / 32, 1 << (order % 32));
			}
			pstr += sprintIndent(indent, tmpStr, "inline bool has%s() { return (_isSetFlag[%d] & 0x%02X) > 0; }\n", uName.c_str(), order / 32, 1 << (order % 32));
		}
		if(!defVal.empty())
		{
			if(!cstr.empty())
				cstr += ", ";
			else
				cstr += ": ";
			cstr += sprintIndent(0, tmpStr, "_%s(%s)", name.c_str(), defVal.c_str());
		}
	}
	else
	{
		if(useRef)
		{
			rstr += sprintIndent(indent, tmpStr, "ssu::vector<%s *> _%s;\n\n", type.c_str(), name.c_str());

			pstr += sprintIndent(indent, tmpStr, "inline const %s& %s(size_t index__) const { return *_%s[index__]; }\n", type.c_str(), lName.c_str(), name.c_str());
			pstr += sprintIndent(indent, tmpStr, "inline %s * add%s() { %s * val__ = new(std::nothrow) %s; if(val__ == NULL) return NULL; _%s.push_back(val__); return val__; }\n", type.c_str(), uName.c_str(), type.c_str(), type.c_str(), name.c_str());
			pstr += sprintIndent(indent, tmpStr, "inline size_t %sSize() const { return _%s.size(); }\n", lName.c_str(), name.c_str());

			dstr += sprintIndent(indent + indentSize, tmpStr, "for(std::vector<%s *>::iterator iter = _%s.begin(); iter != _%s.end(); ++ iter) { delete *iter; }\n", type.c_str(), name.c_str(), name.c_str());
		}
		else
		{
			rstr += sprintIndent(indent, tmpStr, "std::vector<%s> _%s;\n\n", type.c_str(), name.c_str());

			pstr += sprintIndent(indent, tmpStr, "inline %s %s(size_t index__) const { return _%s[index__]; }\n", type.c_str(), lName.c_str(), name.c_str());
			pstr += sprintIndent(indent, tmpStr, "inline void add%s(%s val__) { _%s.push_back(val__); }\n", uName.c_str(), type.c_str(), name.c_str());
			pstr += sprintIndent(indent, tmpStr, "inline void set%s(size_t index__, %s val__) { _%s[index__] = val__; }\n", uName.c_str(), type.c_str(), name.c_str());
			pstr += sprintIndent(indent, tmpStr, "inline size_t %sSize() const { return _%s.size(); }\n", lName.c_str(), name.c_str());
		}
	}
	pstr += "\n";
}

std::string alterDefVal(int type, const std::string& str)
{
	if(!str.empty())
		return str;
	switch(type)
	{
	case TYPE_INT:
	case TYPE_UINT:
	case TYPE_INT64:
	case TYPE_UINT64:
		return "0";
	case TYPE_FLOAT:
		return "0.0f";
	case TYPE_DOUBLE:
		return "0.0";
	case TYPE_BOOL:
		return "false";
	default:
		break;
	}
	return "";
}

void printStruct(std::vector<StructDef *>& sd, int indent)
{
	const char * typeName_[] = { "int", "unsigned int", "long long", "unsigned long long", "float", "double", "std::string", "bool", "std::vector<unsigned char>", "custom" };

	for(auto it = sd.begin(); it != sd.end(); ++ it)
	{
		std::string publicString, protectedString, constructString, destructString;
		fprintIndent(indent, stdout, "class %s\n", (*it)->name.c_str());
		fprintIndent(indent, stdout, "{\n");
		if(!(*it)->structList.empty() || !(*it)->enumList.empty())
		{
			fprintIndent(indent, stdout, "public:\n");
			printEnum((*it)->enumList, indent + indentSize);
			printStruct((*it)->structList, indent + indentSize);
		}
		indent += indentSize;
		bool needFlag = false;
		int maxOrder = 0;
		for(auto it2 = (*it)->fields.begin(); it2 != (*it)->fields.end(); ++ it2)
		{
			if(it2->second->constraint == 2)
			{
				needFlag = true;
				it2->second->order = maxOrder ++;
			}
			if(it2->second->type == TYPE_CUSTOM)
			{
				printField(publicString, protectedString, constructString, destructString, true, it2->second->constraint, it2->second->order, it2->second->tname, it2->second->name, it2->second->defVal, indent);
			}
			else
			{
				switch(it2->second->type)
				{
				case TYPE_STRING:
				case TYPE_VECTOR:
					printField(publicString, protectedString, constructString, destructString, true, it2->second->constraint, it2->second->order, typeName_[it2->second->type], it2->second->name, it2->second->defVal, indent);
					break;
				default:
					printField(publicString, protectedString, constructString, destructString, false, it2->second->constraint, it2->second->order, typeName_[it2->second->type], it2->second->name, alterDefVal(it2->second->type, it2->second->defVal), indent);
					break;
				}
			}
		}
		indent -= indentSize;
		fprintIndent(indent, stdout, "public:\n");
		if(!constructString.empty() || needFlag)
		{
			fprintIndent(indent + indentSize, stdout, "inline %s()", (*it)->name.c_str());
			fputs(constructString.c_str(), stdout);
			if(needFlag)
			{
				fputs("\n", stdout);
				fprintIndent(indent + indentSize, stdout, "{ memset(_isSetFlag, 0, sizeof(_isSetFlag)); }\n\n");
			}
			else
				fputs(" { }\n\n", stdout);
		}
		if(!destructString.empty())
		{
			fprintIndent(indent + indentSize, stdout, "virtual ~%s()\n", (*it)->name.c_str());
			fprintIndent(indent + indentSize, stdout, "{\n");
			fputs(destructString.c_str(), stdout);
			fprintIndent(indent + indentSize, stdout, "}\n\n");
		}
		else
		{
			fprintIndent(indent + indentSize, stdout, "virtual ~%s() { }\n\n", (*it)->name.c_str());
		}
		if(!publicString.empty())
		{
			fprintIndent(indent, stdout, "public:\n");
			fputs(publicString.c_str(), stdout);
		}
		if(!protectedString.empty())
		{
			fprintIndent(indent, stdout, "protected:\n");
			fputs(protectedString.c_str(), stdout);
		}
		if(needFlag)
		{
			fprintIndent(indent + indentSize, stdout, "unsigned int _isSetFlag[%d];\n", (maxOrder + 31) / 32);
			fprintf(stdout, "\n");
		}
		fprintIndent(indent, stdout, "};\n");
	}
	fprintf(stdout, "\n");
}

void process(SSUStruct& ssus)
{
	int indent = 0;
	printEnum(ssus.enumList, 0);
	printStruct(ssus.structList, 0);
}
