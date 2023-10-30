print("Running Premake v" .. _PREMAKE_VERSION)

CPP_VERSION = "C++20"
BASE_PROJECT_NAME = "BasicBindless"
BUILD_DIR = "%{wks.location}/build/%{prj.name}_$(Configuration)_$(Platform)"
INTERMEDIATE_BUILD_DIR = BUILD_DIR .. "Intermediate"
INTERMEDIATE_DIR = "%{wks.location}/Intermediate/"
PROJECT_DIR = "%{wks.location}/%{prj.name}"

function AddSourceFiles(directory_path)
    files
    {
        (directory_path .. "/**.h"),
        (directory_path .. "/**.hpp"),
        (directory_path .. "/**.cpp"),
        (directory_path .. "/**.inl"),
        (directory_path .. "/**.c"),
        (directory_path .. "/**.hlsl"),
        (directory_path .. "/**.cs")
    }
end

function SetupShaderFilters()
    filter { "configurations:Debug" }
        shaderoptions({"/WX", "/Fd $(SolutionDir)Assets\\Shaders\\%{file.basename}.pdb"})
    filter { "configurations:ReleaseWithDebugInfo" }
        shaderoptions({"/WX", "/Fd $(SolutionDir)Assets\\Shaders\\%{file.basename}.pdb"})
    filter { "configurations:Release" }
        shaderoptions({"/WX"})
    filter {}

    filter { "files:**.hlsl" }
        flags "ExcludeFromBuild"
        shaderobjectfileoutput "$(SolutionDir)Assets\\Shaders\\%{file.basename}.cso"
        shadermodel "6.6"
    filter { "files:**_ps.hlsl" }
        removeflags "ExcludeFromBuild"
        shadertype "Pixel"
        shaderentry "Main"
        shadermodel "6.6"
    filter { "files:**_vs.hlsl" }
        removeflags "ExcludeFromBuild"
        shadertype "Vertex"
        shaderentry "Main"
        shadermodel "6.6"
    filter { "files:**_cs.hlsl" }
        removeflags "ExcludeFromBuild"
        shadertype "Compute"
        shaderentry "Main"
        shadermodel "6.6"
    filter {}
end

print("Configuring Solution: " ..BASE_PROJECT_NAME)
solution (BASE_PROJECT_NAME)
    location "./"   -- generate in root
    basedir "./"
    systemversion "latest"
    characterset ("MBCS")
    platforms {"x64"}
    toolset "v143" -- VS2022
    language "C++"
    cppdialect (CPP_VERSION)
    rtti "Off"
    staticruntime "off"
    configurations {"Debug", "ReleaseWithDebugInfo", "Release"}
    warnings "default"

    filter { "configurations:Debug" }
        runtime "Debug"
        defines { "DEBUG" }
        flags { "MultiProcessorCompile" }
        symbols "On"
        optimize "Off"
        debugdir "$(SolutionDir)"

    filter { "configurations:ReleaseWithDebugInfo" }
        runtime "Release"
        defines { "RELEASE", "NDEBUG" }
        flags { "MultiProcessorCompile", "LinkTimeOptimization" }
        symbols "On"
        optimize "Speed"
        debugdir "$(SolutionDir)"

    filter { "configurations:Release" }
        runtime "Release"
        defines { "RELEASE", "NDEBUG" }
        flags { "MultiProcessorCompile", "LinkTimeOptimization" }
        symbols "Off"
        optimize "Speed"

    filter {}

group "Dependencies"
    include "./Source/ThirdParty/SDL2"
    include "./Source/ThirdParty/spdlog"
group ""

group (BASE_PROJECT_NAME)
project_name = BASE_PROJECT_NAME
print("Generating Project: " .. project_name)
project (BASE_PROJECT_NAME)
    targetdir (BUILD_DIR)
    objdir (INTERMEDIATE_BUILD_DIR)
    location (INTERMEDIATE_DIR)
    debugdir ("%{wks.location}")
    kind "ConsoleApp"

    pchheader ( "pch.h" )
    pchsource ("%{wks.location}/Source/pch.cpp")
    forceincludes  { "pch.h" }
    filter "files:**/ThirdParty/**.*"
        flags "NoPCH"
        disablewarnings { "4100" }
    filter {}
    
    SetupShaderFilters()

    AddSourceFiles("%{wks.location}/Source/")
    includedirs { "$(SolutionDir)/Source/", "$(SolutionDir)/Source/ThirdParty" }

    nuget { "Microsoft.Direct3D.D3D12:1.711.3-preview" }

    IncludeSDL2()
    LinkSDL2()
    IncludeSpdlog()

    filter {}

group "Utilities"
project_name = "RegenerateProjectFiles"
print("Generating Project: " .. project_name)
project (project_name)
    kind "Utility"
    targetdir (BUILD_DIR)
    objdir (INTERMEDIATE_BUILD_DIR)
    location (INTERMEDIATE_DIR)

    files
    {
        "%{wks.location}/premake5.lua"
    }

    postbuildmessage "Regenerating project files..."
    postbuildcommands
    {
        "%{wks.location}/GenerateProjectFiles.bat"
    }
    filter {}