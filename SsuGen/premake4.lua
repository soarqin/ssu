project "SsuGen"
    language "C++"
    kind "ConsoleApp"

    files { "Parser.*", "Process.*", "SsuGen.cpp", "LexUtil.h" }

    currPath = path.getdirectory(_SCRIPT)

    configuration "not vs*"
        buildoptions { "-std=gnu++0x" }

    configuration { "Debug" }
        prebuildcommands { currPath .. "/../bin/debug/lemon T=" .. currPath .. "/../lemon/lempar.c " .. currPath .. "/SsuLex.y" }
    configuration { "Release" }
        prebuildcommands { currPath .. "/../bin/release/lemon T=" .. currPath .. "/../lemon/lempar.c " .. currPath .. "/SsuLex.y" }
