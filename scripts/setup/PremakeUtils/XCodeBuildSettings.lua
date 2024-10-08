
function firstToUpper(str)
    return (str:gsub("^%l", string.upper))
end

XCodeBuildSettings = {
    Common = {
        ALWAYS_SEARCH_USER_PATHS = "YES",
        CLANG_ENABLE_OBJC_WEAK = "YES",
        DEAD_CODE_STRIPPING = "YES",
        ONLY_ACTIVE_ARCH = "YES",
        MACOSX_DEPLOYMENT_TARGET = "14.0",
        PRODUCT_BUNDLE_IDENTIFIER = "com.mqttplus"
    },
    Macosx = {},
    Ios = {}
}

function IncludeXCodeBuildSettings()
    
    filter "action:xcode*"

    target = firstToUpper(os.target())
    
    for config, data in pairs(XCodeBuildSettings) do
        if config == target or config == "Common" then
            for key, value in pairs(data) do
                xcodebuildsettings { [key] = value }
            end
        end
    end
end
