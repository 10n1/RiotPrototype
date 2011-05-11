-- A solution contains projects, and defines the available configurations
solution "Riot"
   configurations { "Debug", "Release" }
   platforms { "native", "x32", "x64" }
   location "projects"
 
   -- A project defines one build target
   project "Riot"
      kind "ConsoleApp"
      language "C++"
      files { "**.h", "**.cpp", "**.hlsl", "**.png", "**.dds" }
	  location "projects"
	  includedirs { "src/core", "src/global", "src/gfx", "src/platform", "src/scene", "src", "src/gfx/ogl" }
	  flags { "unicode", "floatfast", "extrawarnings", "enableSSE", "enableSSE2" }
	  debugdir "/"
 
      configuration "Debug"
         defines { "DEBUG", "_DEBUG" }
         flags { "Symbols" }
 
      configuration "Release"
         defines { "NDEBUG" }
         flags { "OptimizeSpeed" }
		 
	 configuration "windows"
		defines "WIN32"
		links { "d3d11", "d3dx11", "d3dcompiler" }
		includedirs { "src/gfx/d3d", "src/platform/win32" }
		excludes "**OSX**"
		flags { "NoMinimalRebuild" }
		buildoptions "/MP"
		
	configuration "macosx"
		excludes "**Win32**"
		includedirs { "src/platform/osx" }
		
		