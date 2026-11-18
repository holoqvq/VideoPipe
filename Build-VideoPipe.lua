dofile("vp_version.lua")

project "VideoPipe"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "off"
   warnings "off"

   targetname "video_pipe"

   targetdir (_MAIN_SCRIPT_DIR .. "/build/" .. OutputDir .. "/bin")
   objdir (_MAIN_SCRIPT_DIR .. "/build/" .. OutputDir .. "/obj/%{prj.name}")

   files
   {
      "nodes/**.h",
      "nodes/**.cpp",
      "objects/**.h",
      "objects/**.cpp",
      "utils/**.h",
      "utils/**.cpp",
      "third_party/tinyexpr/tinyexpr.h",
      "third_party/tinyexpr/tinyexpr.c"
   }

   includedirs
   {
      "utils"
   }

   filter "system:windows"
      systemversion "latest"

      defines
      {
         "_CRT_SECURE_NO_WARNINGS",
         "NOMINMAX",
         "WIN32_LEAN_AND_MEAN"
      }

      includedirs
      {
         _MAIN_SCRIPT_DIR .. "/vcpkg_installed/x64-windows/include/opencv4",
         _MAIN_SCRIPT_DIR .. "/vcpkg_installed/x64-windows/include/gstreamer-1.0",
         _MAIN_SCRIPT_DIR .. "/vcpkg_installed/x64-windows/include/glib-2.0",
         _MAIN_SCRIPT_DIR .. "/vcpkg_installed/x64-windows/lib/glib-2.0/include"
      }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "speed"

    filter "configurations:Dist"
		runtime "Release"
		optimize "speed"
        symbols "off"

   filter {}
