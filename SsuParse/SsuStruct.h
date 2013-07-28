#ifndef _SSUSTRUCT_H_
#define _SSUSTRUCT_H_

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdarg>
#include <map>
#include <string>
#include <vector>

enum FieldType {
  TYPE_INT,
  TYPE_SINT,
  TYPE_UINT,
  TYPE_INT64,
  TYPE_SINT64,
  TYPE_UINT64,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_FIXED32,
  TYPE_FIXED64,
  TYPE_STRING,
  TYPE_BOOL,
  TYPE_ENUM,
  TYPE_STRUCT
};


struct FieldDef {
  FieldDef(): order(0), constraint(0), type(0) {}
  int order;
  int constraint;
  int type;
  std::string tname;
  std::string name;
  std::string defVal;
  std::string comment;
};

struct EnumVal {
  std::string name;
  int val;
  std::string comment;
};

struct EnumDef {
  ~EnumDef() {
    for(auto it = vals.begin(); it != vals.end(); ++ it) {
      delete it->second;
    }
    vals.clear();
  }
  std::string name;
  std::map<int, EnumVal *> vals;
  std::string comment;

  std::map<std::string, EnumVal *> valMap;
};

struct StructDef {
  StructDef(): parent(NULL), id(0) {}
  ~StructDef() {
    for(auto it = structs.begin(); it != structs.end(); ++ it) {
      delete it->second;
    }
    structs.clear();
    for(auto it = enums.begin(); it != enums.end(); ++ it) {
      delete it->second;
    }
    enums.clear();
    for(auto it = fields.begin(); it != fields.end(); ++ it) {
      delete it->second;
    }
    fields.clear();
  }
  std::string name;
  StructDef * parent;
  std::vector<struct StructDef *> structList;
  std::vector<struct EnumDef *> enumList;
  std::map<unsigned int, struct FieldDef *> fields;
  std::string comment;

  std::map<std::string, struct StructDef *> structs;
  std::map<std::string, struct EnumDef *> enums;
  std::map<std::string, struct FieldDef *> fieldMap;

  unsigned int id;
};

struct SSUStruct {
  ~SSUStruct() {
    for(auto it = structs.begin(); it != structs.end(); ++ it) {
      delete it->second;
    }
    structs.clear();
    for(auto it = enums.begin(); it != enums.end(); ++ it) {
      delete it->second;
    }
    enums.clear();
  }

  std::string packageName;
  std::vector<StructDef *> structList;
  std::vector<EnumDef *> enumList;

  std::map<std::string, std::string> options;
  std::map<std::string, StructDef *> structs;
  std::map<std::string, EnumDef *> enums;
};

#endif // _SSUSTRUCT_H_
