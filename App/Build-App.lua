project "App"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files {  "Source/**.h", "Source/**.cpp",
    "../ThirdParty/ImGUI/imgui.cpp",
    "../ThirdParty/ImGUI/imgui_draw.cpp",
    "../ThirdParty/ImGUI/imgui_widgets.cpp",
    "../ThirdParty/ImGUI/imgui_tables.cpp",
    "../ThirdParty/ImGUI/imgui_demo.cpp",
    "../ThirdParty/ImGUI/backends/imgui_impl_opengl3.cpp",
    "../ThirdParty/ImGUI/backends/imgui_impl_glfw.cpp",
    "../ThirdParty/ImGUI/backends/imgui_impl_win32.cpp"}

   includedirs
   {
      "Source",

	  -- Include Core
	  "../Core/Source",

      "../ThirdParty/sqlite",
      
      "../ThirdParty/ImGUI",
      "../ThirdParty/ImGUI/backends",
      "../ThirdParty/ImGUI/examples/libs/glfw/include"
   }

   libdirs
   {
      "../ThirdParty/ImGUI/examples/libs/glfw/lib-vc2010-64"
   }

   links
   {
      "Core",

      "opengl32.lib",
      "glfw3",
      "legacy_stdio_definitions.lib"
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS", "_CRT_SECURE_NO_WARNINGS" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"
       linkoptions { "/NODEFAULTLIB:MSVCRT" }

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"
       linkoptions { "/NODEFAULTLIB:MSVCRT" }

   filter "configurations:Dist"
       defines { "DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"