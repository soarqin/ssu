project "SsuPBCpp"
    language "C++"
    kind "ConsoleApp"

    files { "**.cpp", "**.h" }
    includedirs { "../SsuParse" }
    links { "SsuParse" }

    configuration { "not vs*" }
        buildoptions { "-std=c++11" }
    configuration { "windows", "not vs*" }
        buildoptions { "-static-libgcc", "-static-libstdc++" }
        linkoptions { "-static-libgcc", "-static-libstdc++" }
