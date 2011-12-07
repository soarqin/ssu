@ECHO OFF
..\protobuf\protoc.exe test.ss --cpp_out=.
..\bin\release\SsuGen.exe test.ss test.ssu.cpp test.ssu.h
