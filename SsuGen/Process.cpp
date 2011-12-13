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

#include "Process.h"

int indentSize = 2;

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

void printEnum(FILE * outputFile, std::vector<EnumDef *>& ed, int indent)
{
	for(auto it = ed.begin(); it != ed.end(); ++ it)
	{
		fprintIndent(indent, outputFile, "enum %s\n", (*it)->name.c_str());
		fprintIndent(indent, outputFile, "{\n");
		indent += indentSize;
		for(auto it2 = (*it)->vals.begin(); it2 != (*it)->vals.end(); ++ it2)
		{
			fprintIndent(indent, outputFile, "%s = %d,\n", it2->second->name.c_str(), it2->second->val);
		}
		indent -= indentSize;
		fprintIndent(indent, outputFile, "};\n\n");
	}
}

inline size_t sizeUInt32( unsigned int val )
{
	if(val < (1 << 14))
	{
		if(val < (1 << 7))
			return 1;
		else
			return 2;
	}
	else
	{
		if(val < (1 << 21))
			return 3;
		else if(val < (1 << 28))
			return 4;
		else return 5;
	}
}

void printField(FILE * outputFile, std::string& pstr, std::string& rstr, std::string& cstr, std::string& dstr, std::string& pkstr, std::string& upkstr, std::string& sstr, int constraint, int oorder, int order, int typeId, const std::string& tname, const std::string& name, const std::string& defVal, int indent)
{
	const char * typeName_[] = { "int", "int", "unsigned int", "long long", "long long", "unsigned long long", "float", "double", "std::string", "bool", "std::vector<unsigned char>", "enum", "struct" };
	const char * funcName_[] = { "Int32", "SInt32", "UInt32", "Int64", "SInt64", "UInt64", "Float", "Double", "String", "Bool", "Vector", "Enum", "Referred" };
	int typeInt_[] = {0, 0, 0, 0, 0, 0, 5, 1, 2, 0, 2, 0, 2};
	char tmpStr[1024];

	std::string lName = capitalize(name);
	std::string uName = capitalize(name, true);
	const char * type;
	if(typeId == TYPE_ENUM || typeId == TYPE_STRUCT)
		type = tname.c_str();
	else
		type = typeName_[typeId];
	const char * funcName = funcName_[typeId];
	upkstr += sprintIndent(indentSize * 2, tmpStr, "case %d:\n", oorder);
	if(constraint == 1 || constraint == 2)
	{
		if(typeId == TYPE_STRUCT)
		{
			rstr += sprintIndent(indent, tmpStr, "::ssu::ReferredObject<%s> _%s;\n", type, name.c_str());
		}
		else
		{
			rstr += sprintIndent(indent, tmpStr, "%s _%s;\n", type, name.c_str());
		}
		if(constraint == 1)
			pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::Utils::pack%sTag(buf, %d, _%s);\n", funcName, oorder, name.c_str());
		else
			pkstr += sprintIndent(indentSize, tmpStr, "if(has%s()) buf = ::ssu::Utils::pack%sTag(buf, %d, _%s);\n", uName.c_str(), funcName, oorder, name.c_str());
		upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != %d || !::ssu::Utils::unpack%s(buf, leftSize, _%s)) return false;\n", typeInt_[typeId], funcName, name.c_str());
		if(constraint == 2)
			upkstr += sprintIndent(indentSize * 3, tmpStr, "_isSetFlag[%d] |= 0x%02X;\n", order / 32, 1 << (order % 32));
		if(!sstr.empty())
			sstr += " + ";
		if(constraint == 1)
			sprintf(tmpStr, "%u + ::ssu::Utils::size%s(_%s)", sizeUInt32(oorder << 3), funcName, name.c_str());
		else
			sprintf(tmpStr, "%u + (has%s() ? ::ssu::Utils::size%s(_%s) : 0)", sizeUInt32(oorder << 3), funcName, name.c_str());
		sstr += tmpStr;

		bool useRef = typeId == TYPE_STRUCT || typeId == TYPE_STRING || typeId == TYPE_VECTOR;
		if(constraint == 1)
		{
			if(useRef)
			{
				pstr += sprintIndent(indent, tmpStr, "inline const %s& %s() const { return _%s; }\n", type, lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void set%s(const %s& val__) { _%s = val__; }\n", uName.c_str(), type, name.c_str());
				if(typeId == TYPE_STRING || typeId == TYPE_VECTOR)
					pstr += sprintIndent(indent, tmpStr, "inline %s& mutable%s() { return _%s; }\n", type, uName.c_str(), name.c_str());
				else
					pstr += sprintIndent(indent, tmpStr, "inline %s * mutable%s() { return _%s.getMutable(); }\n", type, uName.c_str(), name.c_str());
			}
			else
			{
				pstr += sprintIndent(indent, tmpStr, "inline %s %s() const { return _%s; }\n", type, lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void set%s(%s val__) { _%s = val__; }\n", uName.c_str(), type, name.c_str());
			}
		}
		else
		{
			if(useRef)
			{
				pstr += sprintIndent(indent, tmpStr, "inline const %s& %s() const { return _%s; }\n", type, lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void set%s(const %s& val__) { _%s = val__; _isSetFlag[%d] |= 0x%02X; }\n", uName.c_str(), type, name.c_str(), order / 32, 1 << (order % 32));
				if(typeId == TYPE_STRING || typeId == TYPE_VECTOR)
					pstr += sprintIndent(indent, tmpStr, "inline %s& mutable%s() { _isSetFlag[%d] |= 0x%02X; return _%s; }\n", type, uName.c_str(), order / 32, 1 << (order % 32), name.c_str());
				else
					pstr += sprintIndent(indent, tmpStr, "inline %s * mutable%s() { _isSetFlag[%d] |= 0x%02X; return _%s.getMutable(); }\n", type, uName.c_str(), order / 32, 1 << (order % 32), name.c_str());
			}
			else
			{
				pstr += sprintIndent(indent, tmpStr, "inline %s %s() const { return _%s; }\n", type, lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void set%s(%s val__) { _%s = val__; _isSetFlag[%d] |= 0x%02X; }\n", uName.c_str(), type, name.c_str(), order / 32, 1 << (order % 32));
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
		int useRef = 0;
		if(typeId == TYPE_STRUCT)
			useRef = 1;
		else if(typeId == TYPE_STRING || typeId == TYPE_VECTOR)
			useRef = 2;
		switch(useRef)
		{
		case 2:
			{
				rstr += sprintIndent(indent, tmpStr, "std::vector<%s> _%s;\n", type, name.c_str());

				pstr += sprintIndent(indent, tmpStr, "inline const %s& %s(size_t index__) const { return _%s[index__]; }\n", type, lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void add%s(const %s& val__) { _%s.push_back(val__); }\n", uName.c_str(), type, name.c_str());
				if(typeId == TYPE_STRING)
				{
					pstr += sprintIndent(indent, tmpStr, "inline void add%s(const char * val__) { _%s.push_back(val__); }\n", uName.c_str(), name.c_str());
					pstr += sprintIndent(indent, tmpStr, "inline void add%s(const char * val__, size_t size__) { _%s.push_back(std::string(val__, size__)); }\n", uName.c_str(), name.c_str());
				}
				pstr += sprintIndent(indent, tmpStr, "inline void set%s(size_t index__, const %s& val__) { _%s[index__] = val__; }\n", uName.c_str(), type, name.c_str());
				if(typeId == TYPE_STRING)
				{
					pstr += sprintIndent(indent, tmpStr, "inline void set%s(size_t index__, const char * val__) { _%s[index__] = val__; }\n", uName.c_str(), name.c_str());
					pstr += sprintIndent(indent, tmpStr, "inline void set%s(size_t index__, const char * val__, size_t size__) { _%s[index__].assign(val__, size__); }\n", uName.c_str(), name.c_str());
				}
				pstr += sprintIndent(indent, tmpStr, "inline %s& mutable%s(size_t index__) { return _%s[index__]; }\n", type, uName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline size_t %sSize() const { return _%s.size(); }\n", lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void clear%s() { _%s.clear(); }\n", uName.c_str(), name.c_str());

				pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::Utils::packRepeatedTag(buf, %d, _%s, ::ssu::Utils::pack%sTag);\n", oorder, name.c_str(), funcName);
				upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != %d || !::ssu::Utils::unpackRepeated(buf, leftSize, _%s, ::ssu::Utils::unpack%s)) return false;\n", typeInt_[typeId], name.c_str(), funcName);
				sprintf(tmpStr, "%u * _%s.size() + ::ssu::Utils::sizeRepeated(_%s, ::ssu::Utils::size%s)", sizeUInt32(oorder << 3), name.c_str(), name.c_str(), funcName);
				if(!sstr.empty())
					sstr += " + ";
				sstr += tmpStr;
			}
			break;
		case 1:
			{
				rstr += sprintIndent(indent, tmpStr, "std::vector<%s *> _%s;\n", type, name.c_str());

				pstr += sprintIndent(indent, tmpStr, "inline const %s& %s(size_t index__) const { return *_%s[index__]; }\n", type, lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline %s * add%s() { %s * val__ = new(std::nothrow) %s; if(val__ == NULL) return NULL; _%s.push_back(val__); return val__; }\n", type, uName.c_str(), type, type, name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline size_t %sSize() const { return _%s.size(); }\n", lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void clear%s() { for(std::vector<%s *>::iterator iter = _%s.begin(); iter != _%s.end(); ++ iter) { delete *iter; } _%s.clear(); }\n", uName.c_str(), type, name.c_str(), name.c_str(), name.c_str());

				dstr += sprintIndent(indentSize, tmpStr, "for(std::vector<%s *>::iterator iter = _%s.begin(); iter != _%s.end(); ++ iter) { delete *iter; }\n", type, name.c_str(), name.c_str());

				pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::Utils::packRepeatedTag(buf, %d, _%s, ::ssu::Utils::packObjectTag<%s>);\n", oorder, name.c_str(), type);
				upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != %d || !::ssu::Utils::unpackRepeatedPtr(buf, leftSize, _%s, ::ssu::Utils::unpackObjectPtr<%s>)) return false;\n", typeInt_[typeId], name.c_str(), type);
				sprintf(tmpStr, "%u * _%s.size() + ::ssu::Utils::sizeRepeated(_%s, ::ssu::Utils::sizeObject<%s>)", sizeUInt32(oorder << 3), name.c_str(), name.c_str(), type);
				if(!sstr.empty())
					sstr += " + ";
				sstr += tmpStr;
			}
			break;
		case 0:
			{
				rstr += sprintIndent(indent, tmpStr, "std::vector<%s> _%s;\n", type, name.c_str());

				pstr += sprintIndent(indent, tmpStr, "inline %s %s(size_t index__) const { return _%s[index__]; }\n", type, lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void add%s(%s val__) { _%s.push_back(val__); }\n", uName.c_str(), type, name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void set%s(size_t index__, %s val__) { _%s[index__] = val__; }\n", uName.c_str(), type, name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline size_t %sSize() const { return _%s.size(); }\n", lName.c_str(), name.c_str());
				pstr += sprintIndent(indent, tmpStr, "inline void clear%s() { _%s.clear(); }\n", uName.c_str(), name.c_str());

				if(!sstr.empty())
					sstr += " + ";
				if(typeId == TYPE_ENUM)
				{
					if(defVal == "true")
					{
						pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::Utils::packRepeatedPackedTag(buf, %d, _%s, ::ssu::Utils::packEnum<%s>, ::ssu::Utils::sizeEnum<%s>);\n", oorder, name.c_str(), type, type);
						upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != 2 || !::ssu::Utils::unpackRepeatedPacked(buf, leftSize, _%s, ::ssu::Utils::unpackEnum<%s>)) return false;\n", name.c_str(), type);
						sprintf(tmpStr, "%u + ::ssu::Utils::sizeRepeatedPacked(_%s, ::ssu::Utils::sizeEnum<%s>)", sizeUInt32(oorder << 3), name.c_str(), type);
						sstr += tmpStr;
					}
					else
					{
						pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::Utils::packRepeatedTag(buf, %d, _%s, ::ssu::Utils::packEnumTag<%s>);\n", oorder, name.c_str(), type);
						upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != %d || !::ssu::Utils::unpackRepeated(buf, leftSize, _%s, ::ssu::Utils::unpackEnum<%s>)) return false;\n", typeInt_[typeId], name.c_str(), type);
						sprintf(tmpStr, "%u * _%s.size() + ::ssu::Utils::sizeRepeated(_%s, ::ssu::Utils::sizeEnum<%s>)", sizeUInt32(oorder << 3), name.c_str(), name.c_str(), type);
						sstr += tmpStr;
					}
				}
				else
				{
					if(defVal == "true")
					{
						pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::Utils::packRepeatedPackedTag(buf, %d, _%s, ::ssu::Utils::pack%s, ::ssu::Utils::size%s);\n", oorder, name.c_str(), funcName, funcName);
						upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != 2 || !::ssu::Utils::unpackRepeatedPacked(buf, leftSize, _%s, ::ssu::Utils::unpack%s)) return false;\n", name.c_str(), funcName);
						sprintf(tmpStr, "%u + ::ssu::Utils::sizeRepeatedPacked(_%s, ::ssu::Utils::size%s)", sizeUInt32(oorder << 3), name.c_str(), funcName);
						sstr += tmpStr;
					}
					else
					{
						pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::Utils::packRepeatedTag(buf, %d, _%s, ::ssu::Utils::pack%sTag);\n", oorder, name.c_str(), funcName);
						upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != %d || !::ssu::Utils::unpackRepeated(buf, leftSize, _%s, ::ssu::Utils::unpack%s)) return false;\n", typeInt_[typeId], name.c_str(), funcName);
						sprintf(tmpStr, "%u * _%s.size() + ::ssu::Utils::sizeRepeated(_%s, ::ssu::Utils::size%s)", sizeUInt32(oorder << 3), name.c_str(), name.c_str(), funcName);
						sstr += tmpStr;
					}
				}
			}
			break;
		}
	}
	upkstr += sprintIndent(indentSize * 3, tmpStr, "break;\n", funcName, oorder, name.c_str());
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

void printStruct(const std::string& parent, FILE * outputFileC, FILE * outputFileH, std::vector<StructDef *>& sd, int indent)
{
	for(auto it = sd.begin(); it != sd.end(); ++ it)
	{
		std::string publicString, protectedString, constructString, destructString, packString, unpackString, sizeString;
		fprintIndent(indent, outputFileH, "class %s: public ::ssu::Object\n", (*it)->name.c_str());
		fprintIndent(indent, outputFileH, "{\n");
		if(!(*it)->structList.empty() || !(*it)->enumList.empty())
		{
			fprintIndent(indent, outputFileH, "public:\n");
			printEnum(outputFileH, (*it)->enumList, indent + indentSize);
			printStruct(parent + (*it)->name + "::", outputFileC, outputFileH, (*it)->structList, indent + indentSize);
		}
		indent += indentSize;
		bool needFlag = false;
		int maxOrder = 0;
		for(auto it2 = (*it)->fields.begin(); it2 != (*it)->fields.end(); ++ it2)
		{
			int oldOrder = it2->second->order;
			if(it2->second->constraint == 2)
			{
				needFlag = true;
				it2->second->order = maxOrder ++;
			}
			printField(outputFileH, publicString, protectedString, constructString, destructString, packString, unpackString, sizeString, it2->second->constraint, oldOrder, it2->second->order, it2->second->type, it2->second->tname, it2->second->name, alterDefVal(it2->second->type, it2->second->defVal), indent);
		}
		indent -= indentSize;
		fprintIndent(indent, outputFileH, "public:\n");
		if(!constructString.empty() || needFlag)
		{
			fprintIndent(indent + indentSize, outputFileH, "inline %s()", (*it)->name.c_str());
			fputs(constructString.c_str(), outputFileH);
			if(needFlag)
			{
				fputs("\n", outputFileH);
				fprintIndent(indent + indentSize, outputFileH, "{ memset(_isSetFlag, 0, sizeof(_isSetFlag)); }\n\n");
			}
			else
				fputs(" { }\n\n", outputFileH);
		}
		if(!destructString.empty())
		{
			fprintIndent(indent + indentSize, outputFileH, "virtual ~%s();\n", (*it)->name.c_str());
			fprintf(outputFileC, "%s%s::~%s()\n", parent.c_str(), (*it)->name.c_str(), (*it)->name.c_str());
			fputs("{\n", outputFileC);
			fputs(destructString.c_str(), outputFileC);
			fputs("}\n\n", outputFileC);
		}
		else
		{
			fprintIndent(indent + indentSize, outputFileH, "virtual ~%s() { }\n\n", (*it)->name.c_str());
		}
		fprintIndent(indent, outputFileH, "public:\n");
		fprintIndent(indent + indentSize, outputFileH, "virtual unsigned char * packBuffer(unsigned char * buf);\n");
		fprintIndent(indent + indentSize, outputFileH, "virtual bool unpackBuffer(const unsigned char *& buf, size_t& leftSize);\n");
		fprintIndent(indent + indentSize, outputFileH, "virtual size_t size();\n\n");
		if(!publicString.empty())
		{
			fprintIndent(indent, outputFileH, "public:\n");
			fputs(publicString.c_str(), outputFileH);
		}
		if(!protectedString.empty())
		{
			fprintIndent(indent, outputFileH, "protected:\n");
			fputs(protectedString.c_str(), outputFileH);
		}
		if(needFlag)
		{
			fprintf(outputFileH, "\n");
			fprintIndent(indent + indentSize, outputFileH, "unsigned int _isSetFlag[%d];\n", (maxOrder + 31) / 32);
		}
		fprintf(outputFileH, "\n");
		fprintIndent(indent, outputFileH, "};\n\n");

		fprintIndent(0, outputFileC, "unsigned char * %s%s::packBuffer(unsigned char * buf)\n", parent.c_str(), (*it)->name.c_str());
		fprintIndent(0, outputFileC, "{\n");
		fputs(packString.c_str(), outputFileC);
		fprintIndent(indentSize, outputFileC, "return buf;\n");
		fprintIndent(0, outputFileC, "}\n\n");
		fprintIndent(0, outputFileC, "bool %s%s::unpackBuffer(const unsigned char *& buf, size_t& leftSize)\n", parent.c_str(), (*it)->name.c_str());
		fprintIndent(0, outputFileC, "{\n");
		if(!unpackString.empty())
		{
			fprintIndent(indentSize, outputFileC, "unsigned int tag_; unsigned char type_;\n");
			fprintIndent(indentSize, outputFileC, "while(leftSize > 0)\n");
			fprintIndent(indentSize, outputFileC, "{\n");
			fprintIndent(indentSize * 2, outputFileC, "if(!::ssu::Utils::unpackTag(buf, leftSize, tag_, type_)) return false;\n");
			fprintIndent(indentSize * 2, outputFileC, "switch(tag_)\n");
			fprintIndent(indentSize * 2, outputFileC, "{\n");
			fputs(unpackString.c_str(), outputFileC);
			fprintIndent(indentSize * 2, outputFileC, "default: break;\n");
			fprintIndent(indentSize * 2, outputFileC, "}\n");
			fprintIndent(indentSize, outputFileC, "}\n");
		}
		fprintIndent(indentSize, outputFileC, "return true;\n");
		fprintIndent(0, outputFileC, "}\n\n");
		fprintIndent(0, outputFileC, "size_t %s%s::size()\n", parent.c_str(), (*it)->name.c_str());
		fprintIndent(0, outputFileC, "{\n");
		if(sizeString.empty())
			fprintIndent(indentSize, outputFileC, "return 0;\n");
		else
			fprintIndent(indentSize, outputFileC, "return %s;\n", sizeString.c_str());
		fprintIndent(0, outputFileC, "}\n\n");

	}
}

void process(FILE * outputFileC, FILE * outputFileH, SSUStruct& ssus)
{
	fprintf(outputFileH, "#include \"SsuObject.h\"\n\n");

	int namespaceLevel = 0;
	if(!ssus.packageName.empty())
	{
		size_t start = 0, i = 0;
		while(i < ssus.packageName.length())
		{
			char c = ssus.packageName[i];
			if(c == '.')
			{
				if(i > start)
				{
					fprintf(outputFileH, "namespace %s {\n", std::string(ssus.packageName.begin() + start, ssus.packageName.begin() + i).c_str());
					fprintf(outputFileC, "namespace %s {\n", std::string(ssus.packageName.begin() + start, ssus.packageName.begin() + i).c_str());
					++ namespaceLevel;
				}
				start = ++ i;
				continue;
			}
			else if(!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_'))
			{
				c = '_';
			}
			++ i;
		}
		fprintf(outputFileH, "namespace %s {\n\n", std::string(ssus.packageName.begin() + start, ssus.packageName.end()).c_str());
		fprintf(outputFileC, "namespace %s {\n\n", std::string(ssus.packageName.begin() + start, ssus.packageName.end()).c_str());
		++ namespaceLevel;
	}
	printEnum(outputFileH, ssus.enumList, 0);
	printStruct("", outputFileC, outputFileH, ssus.structList, 0);

	for(int i = 0; i < namespaceLevel; ++ i)
	{
		fprintf(outputFileH, "}\n");
		fprintf(outputFileC, "}\n");
	}
	fprintf(outputFileH, "\n");
}
