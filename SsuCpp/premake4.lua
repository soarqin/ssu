project "SsuCpp"
    language "C++"
    kind "ConsoleApp"

    files { "Process.*", "SsuGen.cpp", "SsuStruct.h" }
    includedirs { "../SsuParse" }
    links { "SsuParse" }

    configuration "not vs*"
        buildoptions { "-std=gnu++0x" }
