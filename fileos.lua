project "fileos"
	uuid "0218712A-7FC3-4B24-85D6-ACB8FF1A1C56"
	kind "StaticLib"
	language "C++"
	files { 
		"include/**.h",
		"include/**.inl",
		"src/*.h",
		"src/*.cpp",
	}
	includedirs { "include", "../containos/include", "../reflectos/include" }
	objdir( "build/" .. _ACTION )
	location( "build/" .. _ACTION )
	targetdir "lib"
	targetname "%{prj.name}_%{cfg.platform}"

	flags { "FatalWarnings", "NoBufferSecurityCheck", "NoEditAndContinue", "NoIncrementalLink", "NoPCH", "NoRTTI" }
	warnings "Extra"

	configuration "Debug"
		defines { "DEBUG", "_ITERATOR_DEBUG_LEVEL=0" }
		flags { "Symbols" }
		targetsuffix "_d"

	configuration "Release"
		defines { "NDEBUG" }
		flags { "NoRuntimeChecks" }
		optimize "Speed"
		targetsuffix "_r"
