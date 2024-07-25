include "Config.lua"
include "Dependencies.lua"
include "scripts/setup/PremakeUtils/XCodeBuildSettings.lua"

workspace "MQTTPlus"

	startproject (STARTUP_PROJECT)
    	cppdialect (CPP_DIALECT)

	configurations (CONFIGS)

	flags
	{
		"MultiProcessorCompile"
	}
    
	defines {}
 

	filter "system:windows"
        systemversion "latest"
		architecture "x86_64"
        defines
        {
            "MQP_WINDOWS"
        }

	filter "system:macosx"
        architecture "universal"
        defines
        {
            "MQP_MACOS"
        }

	filter "system:linux"
	defines {
	    "MQP_MACOS"
	}
        
    filter "configurations:Debug"
        defines "MQP_DEBUG"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        defines "MQP_DEBUG"
        runtime "Release"
        optimize "on"
 
    filter "configurations:Dist"
        defines "MQP_DIST"
        runtime "Release"
        optimize "on"

    IncludeXCodeBuildSettings()

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "MQTTPlusBroker"

