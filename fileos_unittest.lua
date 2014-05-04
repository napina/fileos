project "fileos_unittest"
	uuid "A3B543E3-94A0-41C9-89AE-C63AAE3DB092"
	kind "ConsoleApp"
	language "C++"
	files {
		"tests/*.h",
		"tests/*.cpp",
	}
	includedirs { "include", "../containos/include", "../unitos/include" }
	links { "fileos", "unitos" }
	objdir( "build/" .. _ACTION )
	targetdir( "build/" .. _ACTION )
	location( "build/" .. _ACTION )

	flags { "FatalWarnings", "NoBufferSecurityCheck", "NoEditAndContinue", "NoIncrementalLink", "NoPCH", "NoRTTI" }
	warnings "Extra"

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "NoRuntimeChecks" }
		optimize "Speed"

	configuration { "vs*"}
		postbuildcommands { "\"$(TargetPath)\"" }