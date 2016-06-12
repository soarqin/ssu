project "SsuParse"
    language "C++"
    kind "StaticLib"

    files { "LexUtil.h", "Parser.*", "SsuStruct.h" }

    currPath = path.getdirectory(_SCRIPT)

    configuration { "not vs*" }
        buildoptions { "-std=c++11" }
    configuration { "windows", "not vs*" }
        buildoptions { "-static-libgcc", "-static-libstdc++" }
        linkoptions { "-static-libgcc", "-static-libstdc++" }
    configuration { "windows" }
        defines { "BUILD_EXPORT" }
    configuration { "Debug" }
        prebuildcommands { currPath .. "/../bin/debug/lemon T=" .. currPath .. "/../lemon/lempar.c " .. currPath .. "/SsuLex.y" }
    configuration { "Release" }
        prebuildcommands { currPath .. "/../bin/release/lemon T=" .. currPath .. "/../lemon/lempar.c " .. currPath .. "/SsuLex.y" }
