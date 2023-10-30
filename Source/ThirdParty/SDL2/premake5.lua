function IncludeSDL2()
    includedirs ("%{wks.location}/Source/ThirdParty/SDL2/include")
end

function LinkSDL2()
    libdirs	("%{wks.location}/Source/ThirdParty/SDL2/lib/x64")

    links
    {
        "SDL2.lib",
        "SDL2main.lib"
    }

    postbuildcommands
    {
        "{COPY} \"$(SolutionDir)Source\\ThirdParty\\SDL2\\lib\\x64\\SDL2.dll\" \"$(OutDir)\""
    }
end
