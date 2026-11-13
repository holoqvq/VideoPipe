dofile("vp_version.lua")

project "VideoPipe"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "off"
   warnings "off"

   targetname "video_pipe"

   targetdir ("../../../build/" .. OutputDir .. "/bin")
   objdir ("../../../build/" .. OutputDir .. "/obj/%{prj.name}")

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
         "%{os.getenv('OpenCV_DIR')}/include",
         "%{os.getenv('GSTREAMER_1_0_ROOT_MSVC_X86_64')}/include/gstreamer-1.0",
         "%{os.getenv('GSTREAMER_1_0_ROOT_MSVC_X86_64')}/include",
         "%{os.getenv('GSTREAMER_1_0_ROOT_MSVC_X86_64')}/include/glib-2.0",
         "%{os.getenv('GSTREAMER_1_0_ROOT_MSVC_X86_64')}/lib/glib-2.0/include"
      }

      libdirs
      {
         "%{os.getenv('GSTREAMER_1_0_ROOT_MSVC_X86_64')}/lib",
         "%{os.getenv('OpenCV_DIR')}/x64/vc16/lib"
      }

      links
      {
         "gstreamer-1.0",
         "gstapp-1.0",
         "gstbase-1.0",
         "gstrtspserver-1.0",
         "gobject-2.0",
         "glib-2.0",
         "intl"
      }

   filter { "system:windows", "configurations:Debug" }
      links { "opencv_world4140d" }

   filter { "system:windows", "configurations:Release or Dist" }
      links { "opencv_world4140" }

   filter "system:linux"
      systemversion "latest"
      pic "On"

      buildoptions
      {
         "-g",
         "-fPIC",
         "-w",
         "-fdiagnostics-color=always",
         "-pthread"
      }

      linkoptions
      {
         "-pthread"
      }

      includedirs
      {
         "/usr/include/opencv4",
         "/usr/include/gstreamer-1.0",
         "/usr/include",
         "/usr/include/glib-2.0",
         "/usr/lib/x86_64-linux-gnu/glib-2.0/include",
         "/usr/include/sysprof-6"
      }

      links
      {
         "opencv_calib3d",
         "opencv_core",
         "opencv_dnn",
         "opencv_features2d",
         "opencv_flann",
         "opencv_gapi",
         "opencv_highgui",
         "opencv_imgcodecs",
         "opencv_imgproc",
         "opencv_ml",
         "opencv_objdetect",
         "opencv_photo",
         "opencv_stitching",
         "opencv_video",
         "opencv_videoio",
         "gstreamer-1.0",
         "gstapp-1.0",
         "gstbase-1.0",
         "gstrtspserver-1.0",
         "gobject-2.0",
         "glib-2.0",
         "m"
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
