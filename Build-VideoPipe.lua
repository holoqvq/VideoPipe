dofile("vp_version.lua")

project "VideoPipe"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "off"
   warnings "off"

   targetname "video_pipe"

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

   defines { "VP_WITH_FFMPEG" }

   includedirs
   {
      "utils"
   }

   targetdir (_MAIN_SCRIPT_DIR .. "/build/" .. OutputDir .. "/bin")
   objdir (_MAIN_SCRIPT_DIR .. "/build/" .. OutputDir .. "/obj/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines {}

   -- Requires: OpenCV >= 4.6, GStreamer >= 1.14.5, FFmpeg (same as CMakeLists.txt)
   filter "system:linux"
       defines {}

       includedirs
       {
           "/usr/include/opencv4",
           "/usr/include/gstreamer-1.0",
           "/usr/include/glib-2.0",
           "/usr/lib/x86_64-linux-gnu/glib-2.0/include",
           "/usr/include/x86_64-linux-gnu"
       }

       links
       {
           "opencv_core",
           "opencv_imgproc",
           "opencv_imgcodecs",
           "opencv_videoio",
           "opencv_highgui",
           "opencv_video",
           "opencv_dnn",
           "opencv_freetype",
           "gstreamer-1.0",
           "gstbase-1.0",
           "gstapp-1.0",
           "gstrtspserver-1.0",
           "gobject-2.0",
           "glib-2.0",
           "avdevice",
           "avformat",
           "avcodec",
           "swscale",
           "swresample",
           "avutil",
           "stdc++fs"
       }

       buildoptions { "-pthread" }
       linkoptions { "-pthread" }

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
