@ECHO OFF
..\protobuf\protoc.exe test.ss --cpp_out=.
..\Release\ssuParser.exe test.ss test.ssu.cpp test.ssu.h
