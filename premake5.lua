workspace "Luna"
    architecture "arm64"
    configurations { "Debug", "Release", "Dist" }

    project "Luna"
        location "Luna"
        kind "StaticLib"
        language "C++"
        cppdialect "C++23"
        targetdir ("bin/%{cfg.buildcfg}/%{prj.name}")
        objdir ("bin-int/%{cfg.buildcfg}/%{prj.name}")

        files
        {
            "%{prj.name}/Source/**.h",
            "%{prj.name}/Source/**.cpp",
            "%{prj.name}/Source/Luna/**.h",
            "%{prj.name}/Source/Luna/**.cpp"
        }

        filter "system:macos"
            defines "LUNA_PLATFORM_MACOS"
            systemversion "latest"

        filter "configurations:Debug"
            defines "LUNA_DEBUG"
            symbols "On"

        filter "configurations:Release"
            defines "LUNA_RELEASE"
            optimize "On"

        filter "configurations:Dist"
            defines "LUNA_DIST"
            optimize "On"

    project "LunaEditor"
        location "LunaEditor"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++23"
        targetdir ("bin/%{cfg.buildcfg}/%{prj.name}")
        objdir ("bin-int/%{cfg.buildcfg}/%{prj.name}")

        files
        {
            "%{prj.name}/Source/**.h",
            "%{prj.name}/Source/**.cpp"
        }

        includedirs
        {
            "Luna/Source"
        }

        links
        {
            "Luna"
        }

        filter "system:macos"
            defines "LUNA_PLATFORM_MACOS"
            systemversion "latest"

        filter "configurations:Debug"
            defines "LUNA_DEBUG"
            symbols "On"

        filter "configurations:Release"
            defines "LUNA_RELEASE"
            optimize "On"

        filter "configurations:Dist"
            defines "LUNA_DIST"
            optimize "On"