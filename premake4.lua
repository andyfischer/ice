
local getcxxflags = premake.gcc.getcxxflags;
function premake.gcc.getcxxflags(cfg)
    local cxxflags = { Cxx0x = "-std=c++0x" }
    local r = getcxxflags(cfg);
    local r2 = table.translate(cfg.flags, cxxflags);
    for _,v in ipairs(r2) do table.insert(r, v) end
    return r;
end
table.insert(premake.fields.flags.allowed, "Cxx0x");

solution "Ice"
    configurations { "Debug", "Release" }
    language "C++"
    flags { "Symbols", "Cxx0x", "NoRTTI", "NoExceptions" }
    targetdir "build"
    objdir "build/obj"
    includedirs { "include", "src" }

    configuration "Release"
        flags { "OptimizeSpeed" }

    configuration "Debug"
        defines { "DEBUG" }

    project "library"
        kind "StaticLib"

        targetname "ice"
        location "src"
        files {
            "src/*.cpp",
            "src/lisp/*.cpp"
        }

        configuration "Debug"
            targetname "ice_d"

    project "unit_tests"
        kind "ConsoleApp"
        location "src"
        files {"src/test/*.cpp"}
        links {"library","dl"}

        configuration "Debug"
            targetname "ice_test"
            
    --[[project "command_line"
        kind "ConsoleApp"
        targetname "ice"
        location "src"
        defines { }
        files {
            "src/command_line/command_line.cpp",
            "src/command_line/command_line_main.cpp",
            "3rdparty/linenoise/linenoise.c",
        }
        links {"library","dl"}

        configuration "Debug"
            targetname "ice"
    ]]--
