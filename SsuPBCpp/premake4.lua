project "SsuPBCpp"
    language "C++"
    kind "ConsoleApp"

    files { "**.cpp", "**.h" }
    includedirs { "../SsuParse" }
    links { "SsuParse" }

    configuration "not vs*"
        buildoptions { "-std=gnu++0x" }
