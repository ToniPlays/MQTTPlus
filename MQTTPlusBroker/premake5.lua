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
      	"src",
		"%{wks.location}/MQTTPlusBroker/vendor/uSockets/src",
        "/opt/homebrew/Cellar/boost/1.85.0/include"
   	}
	filter "system:windows"
	defines {
		"BOOST_USE_WINDOWS_H",
	}
	filter "system:linux"
	links {
		"mariadbcpp",
	}

	References("uSockets")
	References("gsl")
	References("spdlog")
	References("websocketpp")
	References("nlohmann_json")
