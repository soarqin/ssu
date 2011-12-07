project "test"
    language "C++"
    kind "ConsoleApp"

    files { "test.cpp", "protobuf/**.cc", "protobuf/**.h" }

    includedirs { "../include", "protobuf" }

    currPath = path.getdirectory(_SCRIPT)

    configuration "not vs*"
        buildoptions { "-std=gnu++0x" }

    prebuildcommands { currPath .. "/protobuf/protoc --proto_path=" .. currPath .. "/ --cpp_out=" .. currPath .. "/ " .. currPath .. "/test.ss" }
    configuration { "Debug" }
        prebuildcommands { currPath .. "/../bin/debug/SsuGen " .. currPath .. "/test.ss " .. currPath .. "/test.ssu.cpp " .. currPath .. "/test.ssu.h" }
    configuration { "Release" }
        prebuildcommands { currPath .. "/../bin/release/SsuGen " .. currPath .. "/test.ss " .. currPath .. "/test.ssu.cpp " .. currPath .. "/test.ssu.h" }
