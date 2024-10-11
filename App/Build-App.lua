project "App"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.h", "Source/**.cpp",  "../ThirdParty/ImGUI/backends/imgui_impl_glfw.cpp", "../ThirdParty/ImGUI/backends/imgui_impl_opengl3.cpp"}

   includedirs
   {
      "Source",

	  -- Include Core
	  "../Core/Source",

      "../ThirdParty/sqlite",
      "../ThirdParty/ImGUI",
      "../ThirdParty/ImGUI/backends",
      "../ThirdParty/ImGUI/examples/libs/glfw/include/GLFW"
   }

   libdirs
   {
      "../ThirdParty/ImGUI/examples/libs/glfw/lib-vc2010-64"
   }

   links
   {
      "Core",
      "GLFW",
      "opengl32.lib"
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"