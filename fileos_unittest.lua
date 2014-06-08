project "fileos_unittest"
	uuid "A3B543E3-94A0-41C9-89AE-C63AAE3DB092"
	kind "ConsoleApp"
	language "C++"
	files {
		"tests/*.h",
		"tests/*.cpp",
	}
	includedirs { "include", "../containos/include", "../reflectos/include", "../unitos/include" }
	links { "fileos", "unitos" }
	objdir( "build/" .. _ACTION )
	location( "build/" .. _ACTION )
	targetdir( "build/" .. _ACTION )
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

	configuration { "vs*"}
		postbuildcommands { "\"$(TargetPath)\"" }

