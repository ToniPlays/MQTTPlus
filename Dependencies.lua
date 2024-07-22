
Dependencies = {
	uSockets = {
		IncludeDir = "%{wks.location}/MQTTPlusBroker/vendor/uSockets/src",
		LibraryDir = "%{wks.location}/MQTTPlusBroker/vendor/uSockets",
		LinuxLibs = {
			Common = { "%{wks.location}/MQTTPlusBroker/vendor/uSockets/uSockets.a" },	
		},
		MacosLibs = {}
	},
	gsl = {
		IncludeDir = "%{wks.location}/MQTTPlusBroker/vendor/gsl/include",
	},
	spdlog = {
		IncludeDir = "%{wks.location}/MQTTPlusBroker/vendor/spdlog/include",
	}
}



local function LinkLibs(libs)
    for key, value in pairs(libs) do
        if key ~= "Common" then
            filter (string.format("configurations:%s", key))
        end
        for k, lib in pairs(value) do
            links { lib }
        end
        if key ~= "Common" then
            --print("")
        end
    end
    filter {}
end

function References(reference)
    local ref = Dependencies[reference]
    
    if(ref.IncludeDir ~= nil) then
        includedirs { ref.IncludeDir }
    end
    
    if(ref.LibraryDir ~= nil) then
        libdirs { ref.LibraryDir }
    end

    if(ref.CommonLib ~= nil) then
        links { ref.CommonLib }
    end
    
    if(ref.WindowsLibs ~= nil and os.target() == "windows") then
        LinkLibs(ref.WindowsLibs)
    end
    if(ref.MacosLibs ~= nil and os.target() == "macosx") then
        LinkLibs(ref.MacosLibs)
    end
    if(ref.LinuxLibs ~= nil and os.target() == "linux") then
        LinkLibs(ref.LinuxLibs)
    end
end

premake.override(_G, "project", function(base, ...)
	local rval = base(...)
	local args = {...}
	filter "system:ios"
		filename(args[1] .. "-iOS")
	filter {}
	return rval
	end)

premake.override(_G, "workspace", function(base, ...)
	local rval = base(...)
	local args = {...}
	filter "system:ios"
		filename(args[1] .. "-iOS")
	filter {}
	return rval
	end)
