workspace "Impetus"
	architecture "x64"
	startproject "Game"

	configurations {
		"Debug",
		"Release"
    }
    
    flags
	{
		"MultiProcessorCompile"
	}

	defines {
		"NOMINMAX"
    }

outputdir = "%{cfg.buildcfg}"

includedir = {}
includedir["GLFW"] = "Prism/vendor/GLFW/include"
includedir["Vulkan"] = (os.getenv("VK_SDK_PATH") .. "/include")
includedir["VulkanShader"] = (os.getenv("VK_SDK_PATH") .. "/shaderc/libshader/include")
includedir["ImGui"] = "Prism/vendor/imgui"
includedir["spdlog"] = "Prism/vendor/spdlog/include"
includedir["glm"] = "Prism/vendor/glm"
includedir["stb_image"] = "Prism/vendor/stb_image"

group "Dependencies"
	include "Prism/vendor/GLFW"
    --include "Prism/vendor/imgui"
    
group ""


project "Prism"
	location "Prism"
	kind "StaticLib"
    language "C++"
    cppdialect "C++17"
	staticruntime "on"
	systemversion "latest"
	
	targetdir ("bin/" .. outputdir)
	objdir ("bin-int/%{prj.name}-" .. outputdir)

	pchheader "pch.h"
	pchsource "Prism/src/pch.cpp"

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
        "%{prj.name}/src",
        "%{includedir.GLFW}",
        --"%{includedir.ImGui}",
        --"%{includedir.spdlog}",
        --"%{includedir.glm}",
        --"%{includedir.stb_image}",
        "%{includedir.Vulkan}",
        "%{includedir.VulkanShader}"
	}

	links {
        "GLFW",
        --"ImGui",
		(os.getenv("VK_SDK_PATH") .. "/lib/vulkan-1.lib"),
		(os.getenv("VK_SDK_PATH") .. "/lib/shaderc_combined.lib")
	}

	defines {
		"GLFW_INCLUDE_VULKAN"
	}
	
	filter "configurations:Debug"
        defines "Prism_DEBUG"
        runtime "Debug"
		buildoptions "/MT" -- to not conflict with shaderc
		symbols "On"

	filter "configurations:Release"
        defines "Prism_RELEASE"
        runtime "Release"
		optimize "On"


project "Game"
	location "Game"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"
	systemversion "latest"
	
	targetdir ("bin/" .. outputdir)
	objdir ("bin-int/%{prj.name}-" .. outputdir)

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
        "Prism/src"--,
        --"%{includedir.glm}"
	}
	
	links {
		"Prism"
	}
	
	filter "configurations:Debug"
		defines "Prism_DEBUG"
		runtime "Debug"
		buildoptions "/MT" -- to not conflict with shaderc
		symbols "On"

	filter "configurations:Release"
		defines "Prism_RELEASE"
		runtime "Release"
		optimize "On"