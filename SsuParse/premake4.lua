project "SsuParse"
    language "C++"
    kind "SharedLib"

    files { "LexUtil.h", "Parser.*", "SsuStruct.h" }

    currPath = path.getdirectory(_SCRIPT)

    configuration { "not vs*" }
        buildoptions { "-std=gnu++0x" }
    configuration { "windows" }
        defines { "BUILD_EXPORT" }
    configuration { "Debug" }
        prebuildcommands { currPath .. "/../bin/debug/lemon T=" .. currPath .. "/../lemon/lempar.c " .. currPath .. "/SsuLex.y" }
    configuration { "Release" }
        prebuildcommands { currPath .. "/../bin/release/lemon T=" .. currPath .. "/../lemon/lempar.c " .. currPath .. "/SsuLex.y" }
