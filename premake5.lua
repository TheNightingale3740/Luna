workspace "Radiance"
    architecture "arm64"
    configurations { "Debug", "Release", "Dist" }

    project "Radiance"
        location "Radiance"
        kind "StaticLib"
        language "C++"
        cppdialect "C++23"
        targetdir ("bin/%{cfg.buildcfg}/%{prj.name}")
        objdir ("bin-int/%{cfg.buildcfg}/%{prj.name}")

        files
        {
            "%{prj.name}/Source/**.h",
            "%{prj.name}/Source/**.cpp",
            "%{prj.name}/Source/**.mm",
            "%{prj.name}/Source/Radiance/**.h",
            "%{prj.name}/Source/Radiance/**.cpp",
            "%{prj.name}/Source/Radiance/**.mm",
            "Luna/Vendor/imgui/*.cpp",
            "Luna/Vendor/imgui/backends/imgui_impl_glfw.cpp",
            "Luna/Vendor/imgui/backends/imgui_impl_metal.mm"
        }

        includedirs
        {
            "Radiance/Source",
            "Radiance/Vendor/glfw/include",
            "Radiance/Vendor/metal-cpp",
            "Radiance/Vendor/imgui",
            "Radiance/Vendor/spdlog/include",
            "Radiance/Vendor/glm"
        }

        links
        {
            "Radiance/Vendor/glfw/lib/libglfw3.a",
            "Metal.framework",
            "MetalKit.framework",
            "Cocoa.framework",
            "QuartzCore.framework",
            "Foundation.framework",
            "IOKit.framework",
            "CoreVideo.framework"
        }

        filter "system:macos"
            defines "RD_PLATFORM_MACOS"
            systemversion "latest"

            filter "configurations:Debug"
                defines "RD_DEBUG"
                symbols "On"

            filter "configurations:Release"
                defines "RD_RELEASE"
                optimize "On"

            filter "configurations:Dist"
                defines "RD_DIST"
                optimize "On"

    project "RadianceEditor"
        location "RadianceEditor"
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
            "Radiance/Source",
            "Radiance/Vendor/glfw/include",
            "Radiance/Vendor/glm",
            "Radiance/Vendor/imgui",
            "Radiance/Vendor/metal-cpp"
        }

        libdirs
        {
            "Radiance/Vendor/glfw/lib"
        }

        links
        {
            "Radiance",
            "glfw3",
            "Metal.framework",
            "MetalKit.framework",
            "Cocoa.framework",
            "QuartzCore.framework",
            "Foundation.framework",
            "IOKit.framework",
            "CoreVideo.framework"
        }

        filter "system:macos"
            defines "RD_PLATFORM_MACOS"
            systemversion "latest"

        filter "configurations:Debug"
            defines "RD_DEBUG"
            symbols "On"

        filter "configurations:Release"
            defines "RD_RELEASE"
            optimize "On"

        filter "configurations:Dist"
            defines "RD_DIST"
            optimize "On"