project "MQTTPlusBroker"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

    	includedirs
    	{
        	"src"
    	}
	defines {
	
	}

	References("uSockets")
	References("gsl")
	References("spdlog")
	References("websocketpp")
	References("nlohmann_json")