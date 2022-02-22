workspace "Game Engine"
   configurations { "Debug", "Release" }
   platforms { "Win64" }

filter "configurations:Debug"
   defines { "DEBUG" }
   symbols "On"

filter "configurations:Release"
   defines { "NDEBUG" }
   optimize "On"
   
filter { "platforms:Win64" }
   system "Windows"
   architecture "x86_64"

project "game"
   location "game"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{prj.name}/%{cfg.buildcfg}"
   files { "%{prj.location}/src/**.h", "%{prj.location}/src/**.cpp" }
   objdir "bin/ojb/%{prj.name}/%{cfg.buildcfg}"
   links { "engine", "lodepng", "d3d12.lib", "d3dcompiler.lib", "dxgi.lib", "dxguid.lib", "graphics" }
   includedirs { "game/src", "engine/src/include", "lib/spdlog", "lib/glm" }
   debugdir "build"
   postbuildcommands {
	"{MKDIR} %{wks.location}/build",
	"{COPY} %{cfg.buildtarget.abspath} %{wks.location}/build"
	}
   
project "engine"
   location "engine"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{prj.name}/%{cfg.buildcfg}"
   files { "%{prj.location}/src/**.h", "%{prj.location}/src/**.cpp" }
   objdir "bin/ojb/%{prj.name}/%{cfg.buildcfg}"
   includedirs { "engine/src", "lib/json", "lib/lodepng", "lib/spdlog", "lib/glm", "graphics/include", "lib/entt/include", "lib/aftermath/include" }
   pchheader "pch.h"
   pchsource "%{prj.location}/src/pch.cpp"
   
project "graphics"
   location "graphics"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{prj.name}/%{cfg.buildcfg}"
   files { "%{prj.location}/src/**.h", "%{prj.location}/src/**.cpp" }
   objdir "bin/obj/%{prj.name}/%{cfg.buildcfg}"
   includedirs { "graphics/src", "lib/aftermath/include" }
   
project "editor"
   location "editor"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{prj.name}/%{cfg.buildcfg}"
   files { "%{prj.location}/src/**.h", "%{prj.location}/src/**.cpp" }
   objdir "bin/obj/%{prj.name}/%{cfg.buildcfg}"
   includedirs { "engine/src/include", "lib/json", "lib/lodepng", "lib/spdlog", "lib/glm", "graphics/include", "graphics/src", "lib/entt/include", "engine/src" }
   links { "engine", "lodepng", "d3d12.lib", "d3dcompiler.lib", "dxgi.lib", "dxguid.lib", "graphics" }
   pchheader "pch.h"
   pchsource "%{prj.location}/src/pch.cpp"
   debugdir "build"
   postbuildcommands {
	"{MKDIR} %{wks.location}/build",
	"{COPY} %{cfg.buildtarget.abspath} %{wks.location}/build"
	}
   
project "wrapper"
   location "wrapper"
   kind "SharedLib"
   language "C++"
   targetdir "bin/%{prj.name}/%{cfg.buildcfg}"
   files { "%{prj.location}/src/**.h", "%{prj.location}/src/**.cpp" }
   objdir "bin/ojb/%{prj.name}/%{cfg.buildcfg}"
   includedirs { "wrapper/src", "engine/src/include" }
   clr "on"
   links { "engine", "lodepng", "d3d11.lib", "d3dcompiler.lib" }
   
project "lodepng"
	location "lib/lodepng"
	kind "StaticLib"
	language "C++"
	targetdir "bin/%{prj.name}/%{cfg.buildcfg}"
	files { "%{prj.location}/**.h", "%{prj.location}/**.cpp" }
	objdir "bin/ojb/%{prj.name}/%{cfg.buildcfg}"
	
project "tinyxml"
	location "lib/tinyxml"
	kind "StaticLib"
	language "C++"
	targetdir "bin/%{prj.name}/%{cfg.buildcfg}"
	files { "%{prj.location}/src/**.h", "%{prj.location}/src/**.cpp" }
	objdir "bin/ojb/%{prj.name}/%{cfg.buildcfg}"
	
project "qmfconvert"
   location "tools/qmfconvert"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{prj.name}/%{cfg.buildcfg}"
   files { "%{prj.location}/src/**.h", "%{prj.location}/src/**.cpp" }
   objdir "bin/ojb/%{prj.name}/%{cfg.buildcfg}"
   links { "tinyxml" }
   includedirs { "lib/tinyxml/src" }