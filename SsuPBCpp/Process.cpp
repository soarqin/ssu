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

int indentSize = 4;

std::string capitalize(const std::string& oldName, bool capitalFirst = false) {
    std::string result;
    bool needCapital = capitalFirst;
    auto it = oldName.begin();
    while(it != oldName.end() && (*it) == '_')
        ++ it;
    for(; it != oldName.end(); ++ it) {
        if((*it) == '_')
            needCapital = true;
        else {
            if(needCapital) {
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

void fprintIndent(int indent, FILE * fp, const char * pattern, ...) {
    va_list argp;
    va_start(argp, pattern);
    for(int i = 0; i < indent; ++ i)
        fputc(' ', fp);
    vfprintf(fp, pattern, argp);
    va_end(argp);
}

char * sprintIndent(int indent, char * str, const char * pattern, ...) {
    va_list argp;
    memset(str, ' ', indent);
    str[indent] = 0;
    va_start(argp, pattern);
    vsprintf(str + indent, pattern, argp);
    va_end(argp);
    return str;
}

void printEnum(FILE * outputFile, std::vector<EnumDef *>& ed, int indent) {
    for(auto it = ed.begin(); it != ed.end(); ++ it) {
        fprintIndent(indent, outputFile, "enum %s\n {", (*it)->name.c_str());
        indent += indentSize;
        for(auto it2 = (*it)->vals.begin(); it2 != (*it)->vals.end(); ++ it2) {
            fprintIndent(indent, outputFile, "%s = %d,\n", it2->second->name.c_str(), it2->second->val);
        }
        indent -= indentSize;
        fprintIndent(indent, outputFile, "};\n\n");
    }
}

inline size_t sizeUInt32( uint32_t val ) {
    if(val < (1 << 14)) {
        if(val < (1 << 7))
            return 1;
        else
            return 2;
    }
    else {
        if(val < (1 << 21))
            return 3;
        else if(val < (1 << 28))
            return 4;
        else return 5;
    }
}

void printField(FILE * outputFile, std::string& pstr, std::string& rstr, std::string& cstr, std::string& dstr, std::string& pkstr, std::string& upkstr, std::string& sstr, int constraint, int oorder, int order, int typeId, const std::string& tname, const std::string& name, const std::string& defVal, int indent) {
    const char * typeName_[] = { "int", "int", "uint32_t", "int64_t", "int64_t", "uint64_t", "float", "double", "int", "int64_t", "std::string", "bool", "enum", "struct" };
    const char * funcName_[] = { "Int32", "SInt32", "UInt32", "Int64", "SInt64", "UInt64", "Float", "Double", "Fixed32", "Fixed64", "String", "Bool", "Enum", "Referred" };
    int typeInt_[] = {0, 0, 0, 0, 0, 0, 5, 1, 5, 1, 2, 0, 0, 2};
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
    if(constraint == 1 || constraint == 2) {
        if(typeId == TYPE_STRUCT) {
            rstr += sprintIndent(indent, tmpStr, "mutable ::ssu::ReferredObject<%s> _%s;\n", type, name.c_str());
        }
        else {
            rstr += sprintIndent(indent, tmpStr, "%s _%s;\n", type, name.c_str());
        }
        if(constraint == 1)
            pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::utils::Pack%sTag(buf, %d, _%s);\n", funcName, oorder, name.c_str());
        else
            pkstr += sprintIndent(indentSize, tmpStr, "if(Has%s()) buf = ::ssu::utils::Pack%sTag(buf, %d, _%s);\n", uName.c_str(), funcName, oorder, name.c_str());
        upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != %d || !::ssu::utils::Unpack%s(buf, leftSize, _%s)) return false;\n", typeInt_[typeId], funcName, name.c_str());
        if(constraint == 2)
            upkstr += sprintIndent(indentSize * 3, tmpStr, "_isSetFlag[%d] |= 0x%02X;\n", order / 32, 1 << (order % 32));
        if(!sstr.empty())
            sstr += " + ";
        if(constraint == 1)
            sprintf(tmpStr, "%lu + ::ssu::utils::Size%s(_%s)", sizeUInt32(oorder << 3), funcName, name.c_str());
        else
            sprintf(tmpStr, "(Has%s() ? (%lu + ::ssu::utils::Size%s(_%s)) : 0)", uName.c_str(), sizeUInt32(oorder << 3), funcName, name.c_str());
        sstr += tmpStr;

        bool useRef = typeId == TYPE_STRUCT || typeId == TYPE_STRING;
        if(constraint == 1) {
            if(useRef) {
                pstr += sprintIndent(indent, tmpStr, "inline const %s& %s() const { return _%s; }\n", type, uName.c_str(), name.c_str());
                if(typeId != TYPE_STRUCT)
                    pstr += sprintIndent(indent, tmpStr, "inline void Set%s(const %s& val__) { _%s = val__; }\n", uName.c_str(), type, name.c_str());
                if(typeId == TYPE_STRING)
                    pstr += sprintIndent(indent, tmpStr, "inline %s& Mutable%s() { return _%s; }\n", type, uName.c_str(), name.c_str());
                else
                    pstr += sprintIndent(indent, tmpStr, "inline %s * Mutable%s() { return _%s.GetMutable(); }\n", type, uName.c_str(), name.c_str());
            }
            else {
                pstr += sprintIndent(indent, tmpStr, "inline %s %s() const { return _%s; }\n", type, uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline void Set%s(%s val__) { _%s = val__; }\n", uName.c_str(), type, name.c_str());
            }
        }
        else {
            if(useRef) {
                pstr += sprintIndent(indent, tmpStr, "inline const %s& %s() const { return _%s; }\n", type, uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline void Set%s(const %s& val__) { _%s = val__; _isSetFlag[%d] |= 0x%02X; }\n", uName.c_str(), type, name.c_str(), order / 32, 1 << (order % 32));
                if(typeId == TYPE_STRING)
                    pstr += sprintIndent(indent, tmpStr, "inline %s& Mutable%s() { _isSetFlag[%d] |= 0x%02X; return _%s; }\n", type, uName.c_str(), order / 32, 1 << (order % 32), name.c_str());
                else
                    pstr += sprintIndent(indent, tmpStr, "inline %s * Mutable%s() { _isSetFlag[%d] |= 0x%02X; return _%s.GetMutable(); }\n", type, uName.c_str(), order / 32, 1 << (order % 32), name.c_str());
            }
            else {
                pstr += sprintIndent(indent, tmpStr, "inline %s %s() const { return _%s; }\n", type, uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline void Set%s(%s val__) { _%s = val__; _isSetFlag[%d] |= 0x%02X; }\n", uName.c_str(), type, name.c_str(), order / 32, 1 << (order % 32));
            }
            pstr += sprintIndent(indent, tmpStr, "inline bool Has%s() const { return (_isSetFlag[%d] & 0x%02X) > 0; }\n", uName.c_str(), order / 32, 1 << (order % 32));
        }
        if(!defVal.empty()) {
            if(!cstr.empty())
                cstr += ", ";
            else
                cstr += ": ";
            cstr += sprintIndent(0, tmpStr, "_%s(%s)", name.c_str(), defVal.c_str());
        }
    }
    else {
        int useRef = 0;
        if(typeId == TYPE_STRUCT)
            useRef = 1;
        else if(typeId == TYPE_STRING)
            useRef = 2;
        switch(useRef) {
        case 2: {
                rstr += sprintIndent(indent, tmpStr, "::ssu::RepeatedObject<%s *> _%s;\n", type, name.c_str());

                pstr += sprintIndent(indent, tmpStr, "inline const %s& %s(size_t index__) const { return *_%s[index__]; }\n", type, uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline %s * New%s() { return new(std::nothrow) %s; }\n", type, uName.c_str(), type);
                pstr += sprintIndent(indent, tmpStr, "inline void Add%s(const %s& val__) { %s * val2__ = new(std::nothrow) %s(val__); if(val2__ == NULL) return; _%s.Add(val2__); }\n", uName.c_str(), type, type, type, name.c_str());
                if(typeId == TYPE_STRING) {
                    pstr += sprintIndent(indent, tmpStr, "inline void Add%s(const char * val__) { std::string * val2__ = new(std::nothrow) std::string(val__); if(val2__ == NULL) return; _%s.Add(val2__); }\n", uName.c_str(), name.c_str());
                    pstr += sprintIndent(indent, tmpStr, "inline void Add%s(const char * val__, size_t size__) { std::string * val2__ = new(std::nothrow) std::string(val__, size__); if(val2__ == NULL) return; _%s.Add(val2__); }\n", uName.c_str(), name.c_str());
                }
                pstr += sprintIndent(indent, tmpStr, "inline void Set%s(size_t index__, const %s& val__) { _%s[index__]->assign(val__); }\n", uName.c_str(), type, name.c_str());
                if(typeId == TYPE_STRING) {
                    pstr += sprintIndent(indent, tmpStr, "inline void Set%s(size_t index__, const char * val__) { _%s[index__]->assign(val__); }\n", uName.c_str(), name.c_str());
                    pstr += sprintIndent(indent, tmpStr, "inline void Set%s(size_t index__, const char * val__, size_t size__) { _%s[index__]->assign(val__, size__); }\n", uName.c_str(), name.c_str());
                }
                pstr += sprintIndent(indent, tmpStr, "inline %s& Mutable%s(size_t index__) const { return *_%s[index__]; }\n", type, uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline ::ssu::RepeatedObject<%s *>& Mutable%s() { return _%s; }\n", type, uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline size_t %sSize() const { return _%s.Size(); }\n", uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline void Clear%s() { for(::ssu::RepeatedObject<%s *>::iterator iter = _%s.begin(); iter != _%s.end(); ++ iter) { delete *iter; } _%s.Clear(); }\n", uName.c_str(), type, name.c_str(), name.c_str(), name.c_str());

                dstr += sprintIndent(indentSize, tmpStr, "for(::ssu::RepeatedObject<%s *>::iterator iter = _%s.begin(); iter != _%s.end(); ++ iter) { delete *iter; }\n", type, name.c_str(), name.c_str());

                pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::utils::PackRepeatedTag(buf, %d, _%s, ::ssu::utils::Pack%sPtrTag);\n", oorder, name.c_str(), funcName);
                upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != %d || !::ssu::utils::UnpackRepeated(buf, leftSize, _%s, ::ssu::utils::Unpack%sPtr)) return false;\n", typeInt_[typeId], name.c_str(), funcName);
                sprintf(tmpStr, "%lu * _%s.Size() + ::ssu::utils::SizeRepeated(_%s, ::ssu::utils::Size%sPtr)", sizeUInt32(oorder << 3), name.c_str(), name.c_str(), funcName);
                if(!sstr.empty())
                    sstr += " + ";
                sstr += tmpStr;
            }
            break;
        case 1: {
                rstr += sprintIndent(indent, tmpStr, "::ssu::RepeatedObject<%s *> _%s;\n", type, name.c_str());

                pstr += sprintIndent(indent, tmpStr, "inline const %s& %s(size_t index__) const { return *_%s[index__]; }\n", type, uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline %s * New%s() { return new(std::nothrow) %s; }\n", type, uName.c_str(), type);
                pstr += sprintIndent(indent, tmpStr, "inline %s * Add%s() { %s * val__ = new(std::nothrow) %s; if(val__ == NULL) return NULL; _%s.Add(val__); return val__; }\n", type, uName.c_str(), type, type, name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline ::ssu::RepeatedObject<%s *>& Mutable%s() { return _%s; }\n", type, uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline size_t %sSize() const { return _%s.Size(); }\n", uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline void Clear%s() { for(::ssu::RepeatedObject<%s *>::iterator iter = _%s.begin(); iter != _%s.end(); ++ iter) { delete *iter; } _%s.Clear(); }\n", uName.c_str(), type, name.c_str(), name.c_str(), name.c_str());

                dstr += sprintIndent(indentSize, tmpStr, "for(::ssu::RepeatedObject<%s *>::iterator iter = _%s.begin(); iter != _%s.end(); ++ iter) { delete *iter; } _%s.Clear();\n", type, name.c_str(), name.c_str(), name.c_str());

                pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::utils::PackRepeatedTag(buf, %d, _%s, ::ssu::utils::PackObjectTag<%s>);\n", oorder, name.c_str(), type);
                upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != %d || !::ssu::utils::UnpackRepeatedPtr(buf, leftSize, _%s, ::ssu::utils::UnpackObjectPtr<%s>)) return false;\n", typeInt_[typeId], name.c_str(), type);
                sprintf(tmpStr, "%lu * _%s.Size() + ::ssu::utils::SizeRepeated(_%s, ::ssu::utils::SizeObject<%s>)", sizeUInt32(oorder << 3), name.c_str(), name.c_str(), type);
                if(!sstr.empty())
                    sstr += " + ";
                sstr += tmpStr;
            }
            break;
        case 0: {
                rstr += sprintIndent(indent, tmpStr, "::ssu::RepeatedObject<%s> _%s;\n", type, name.c_str());

                pstr += sprintIndent(indent, tmpStr, "inline %s %s(size_t index__) const { return _%s[index__]; }\n", type, uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline void Add%s(%s val__) { _%s.Add(val__); }\n", uName.c_str(), type, name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline void Set%s(size_t index__, %s val__) { _%s[index__] = val__; }\n", uName.c_str(), type, name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline ::ssu::RepeatedObject<%s>& Mutable%s() { return _%s; }\n", type, uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline size_t %sSize() const { return _%s.Size(); }\n", uName.c_str(), name.c_str());
                pstr += sprintIndent(indent, tmpStr, "inline void Clear%s() { _%s.Clear(); }\n", uName.c_str(), name.c_str());

                if(!sstr.empty())
                    sstr += " + ";
                if(typeId == TYPE_ENUM) {
                    if(defVal == "true") {
                        pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::utils::PackRepeatedPackedTag(buf, %d, _%s, ::ssu::utils::PackEnum<%s>, ::ssu::utils::SizeEnum<%s>);\n", oorder, name.c_str(), type, type);
                        upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != 2 || !::ssu::utils::UnpackRepeatedPacked(buf, leftSize, _%s, ::ssu::utils::UnpackEnum<%s>)) return false;\n", name.c_str(), type);
                        sprintf(tmpStr, "%lu + ::ssu::utils::SizeRepeatedPacked(_%s, ::ssu::utils::SizeEnum<%s>)", sizeUInt32(oorder << 3), name.c_str(), type);
                        sstr += tmpStr;
                    }
                    else {
                        pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::utils::PackRepeatedTag(buf, %d, _%s, ::ssu::utils::PackEnumTag<%s>);\n", oorder, name.c_str(), type);
                        upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != %d || !::ssu::utils::UnpackRepeated(buf, leftSize, _%s, ::ssu::utils::UnpackEnum<%s>)) return false;\n", typeInt_[typeId], name.c_str(), type);
                        sprintf(tmpStr, "%lu * _%s.Size() + ::ssu::utils::SizeRepeated(_%s, ::ssu::utils::SizeEnum<%s>)", sizeUInt32(oorder << 3), name.c_str(), name.c_str(), type);
                        sstr += tmpStr;
                    }
                }
                else {
                    if(defVal == "true") {
                        pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::utils::PackRepeatedPackedTag(buf, %d, _%s, ::ssu::utils::Pack%s, ::ssu::utils::Size%s);\n", oorder, name.c_str(), funcName, funcName);
                        upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != 2 || !::ssu::utils::UnpackRepeatedPacked(buf, leftSize, _%s, ::ssu::utils::Unpack%s)) return false;\n", name.c_str(), funcName);
                        sprintf(tmpStr, "%lu + ::ssu::utils::SizeRepeatedPacked(_%s, ::ssu::utils::Size%s)", sizeUInt32(oorder << 3), name.c_str(), funcName);
                        sstr += tmpStr;
                    }
                    else {
                        pkstr += sprintIndent(indentSize, tmpStr, "buf = ::ssu::utils::PackRepeatedTag(buf, %d, _%s, ::ssu::utils::Pack%sTag);\n", oorder, name.c_str(), funcName);
                        upkstr += sprintIndent(indentSize * 3, tmpStr, "if(type_ != %d || !::ssu::utils::UnpackRepeated(buf, leftSize, _%s, ::ssu::utils::Unpack%s)) return false;\n", typeInt_[typeId], name.c_str(), funcName);
                        sprintf(tmpStr, "%lu * _%s.Size() + ::ssu::utils::SizeRepeated(_%s, ::ssu::utils::Size%s)", sizeUInt32(oorder << 3), name.c_str(), name.c_str(), funcName);
                        sstr += tmpStr;
                    }
                }
            }
            break;
        }
        pstr += sprintIndent(indent, tmpStr, "inline void Reserve%s(size_t size__) { if(_%s.Size() < size__) _%s.Reserve(size__); }\n", uName.c_str(), name.c_str(), name.c_str());
    }
    upkstr += sprintIndent(indentSize * 3, tmpStr, "break;\n", funcName, oorder, name.c_str());
    pstr += "\n";
}

std::string alterDefVal(int type, const std::string& str) {
    if(!str.empty())
        return str;
    switch(type) {
     case TYPE_INT:
     case TYPE_SINT:
     case TYPE_UINT:
     case TYPE_INT64:
     case TYPE_SINT64:
     case TYPE_UINT64:
     case TYPE_FIXED32:
     case TYPE_FIXED64:
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

void printStruct(const std::string& parent, FILE * outputFileC, FILE * outputFileH, std::vector<StructDef *>& sd, int indent) {
    for(auto it = sd.begin(); it != sd.end(); ++ it) {
        std::string publicString, protectedString, constructString, destructString, packString, unpackString, sizeString;
        fprintIndent(indent, outputFileH, "class %s: public ::ssu::Object {\n", (*it)->name.c_str());
        if(!(*it)->structList.empty() || !(*it)->enumList.empty()) {
            fprintIndent(indent, outputFileH, "public:\n");
            printEnum(outputFileH, (*it)->enumList, indent + indentSize);
            printStruct(parent + (*it)->name + "::", outputFileC, outputFileH, (*it)->structList, indent + indentSize);
        }
        indent += indentSize;
        bool needFlag = false;
        int maxOrder = 0;
        for(auto it2 = (*it)->fields.begin(); it2 != (*it)->fields.end(); ++ it2) {
            int oldOrder = it2->second->order;
            if(it2->second->constraint == 2) {
                needFlag = true;
                it2->second->order = maxOrder ++;
            }
            printField(outputFileH, publicString, protectedString, constructString, destructString, packString, unpackString, sizeString, it2->second->constraint, oldOrder, it2->second->order, it2->second->type, it2->second->tname, it2->second->name, alterDefVal(it2->second->type, it2->second->defVal), indent);
        }
        indent -= indentSize;
        fprintIndent(indent, outputFileH, "public:\n");
        if(!constructString.empty() || needFlag) {
            fprintIndent(indent + indentSize, outputFileH, "inline %s()", (*it)->name.c_str());
            fputs(constructString.c_str(), outputFileH);
            if(needFlag) {
                fputs("\n", outputFileH);
                fprintIndent(indent + indentSize, outputFileH, "{ memset(_isSetFlag, 0, sizeof(_isSetFlag)); }\n\n");
            }
            else
                fputs(" { }\n\n", outputFileH);
        }
        if(!destructString.empty()) {
            fprintIndent(indent + indentSize, outputFileH, "virtual ~%s();\n", (*it)->name.c_str());
            fprintf(outputFileC, "%s%s::~%s() {\n", parent.c_str(), (*it)->name.c_str(), (*it)->name.c_str());
            fputs(destructString.c_str(), outputFileC);
            fputs("}\n\n", outputFileC);
        }
        else {
            fprintIndent(indent + indentSize, outputFileH, "virtual ~%s() { }\n\n", (*it)->name.c_str());
        }
        fprintIndent(indent, outputFileH, "public:\n");
        fprintIndent(indent + indentSize, outputFileH, "virtual uint8_t * PackBuffer(uint8_t * buf);\n");
        fprintIndent(indent + indentSize, outputFileH, "virtual bool UnpackBuffer(const uint8_t *& buf, size_t& leftSize);\n");
        fprintIndent(indent + indentSize, outputFileH, "virtual size_t Size() const;\n\n");
        if(!publicString.empty()) {
            fprintIndent(indent, outputFileH, "public:\n");
            fputs(publicString.c_str(), outputFileH);
        }
        if(!protectedString.empty()) {
            fprintIndent(indent, outputFileH, "protected:\n");
            fputs(protectedString.c_str(), outputFileH);
        }
        if(needFlag) {
            fprintf(outputFileH, "\n");
            fprintIndent(indent + indentSize, outputFileH, "uint32_t _isSetFlag[%d];\n", (maxOrder + 31) / 32);
        }
        fprintf(outputFileH, "\n");
        fprintIndent(indent, outputFileH, "};\n\n");

        fprintIndent(0, outputFileC, "uint8_t * %s%s::PackBuffer(uint8_t * buf) {\n", parent.c_str(), (*it)->name.c_str());
        fputs(packString.c_str(), outputFileC);
        fprintIndent(indentSize, outputFileC, "return buf;\n");
        fprintIndent(0, outputFileC, "}\n\n");
        fprintIndent(0, outputFileC, "bool %s%s::UnpackBuffer(const uint8_t *& buf, size_t& leftSize) {\n", parent.c_str(), (*it)->name.c_str());
        if(!unpackString.empty()) {
            fprintIndent(indentSize, outputFileC, "uint32_t tag_; uint8_t type_;\n");
            fprintIndent(indentSize, outputFileC, "while(leftSize > 0) {\n");
            fprintIndent(indentSize * 2, outputFileC, "if(!::ssu::utils::UnpackTag(buf, leftSize, tag_, type_)) return false;\n");
            fprintIndent(indentSize * 2, outputFileC, "switch(tag_) {\n");
            fputs(unpackString.c_str(), outputFileC);
            fprintIndent(indentSize * 2 + 1, outputFileC, "default: break;\n");
            fprintIndent(indentSize * 2, outputFileC, "}\n");
            fprintIndent(indentSize, outputFileC, "}\n");
        }
        fprintIndent(indentSize, outputFileC, "return true;\n");
        fprintIndent(0, outputFileC, "}\n\n");
        fprintIndent(0, outputFileC, "size_t %s%s::Size() const {\n", parent.c_str(), (*it)->name.c_str());
        if(sizeString.empty())
            fprintIndent(indentSize, outputFileC, "return 0;\n");
        else
            fprintIndent(indentSize, outputFileC, "return %s;\n", sizeString.c_str());
        fprintIndent(0, outputFileC, "}\n\n");

    }
}

void process(FILE * outputFileC, FILE * outputFileH, SSUStruct& ssus) {
    fprintf(outputFileH, "#include <stdint.h>\n");
    fprintf(outputFileH, "#include \"SsuObject.h\"\n\n");

    int namespaceLevel = 0;
    if(!ssus.packageName.empty()) {
        size_t start = 0, i = 0;
        while(i < ssus.packageName.length()) {
            char c = ssus.packageName[i];
            if(c == '.') {
                if(i > start) {
                    fprintf(outputFileH, "namespace %s {\n", std::string(ssus.packageName.begin() + start, ssus.packageName.begin() + i).c_str());
                    fprintf(outputFileC, "namespace %s {\n", std::string(ssus.packageName.begin() + start, ssus.packageName.begin() + i).c_str());
                    ++ namespaceLevel;
                }
                start = ++ i;
                continue;
            }
            else if(!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')) {
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

    for(int i = 0; i < namespaceLevel; ++ i) {
        fprintf(outputFileH, "}\n");
        fprintf(outputFileC, "}\n");
    }
    fprintf(outputFileH, "\n");
}
