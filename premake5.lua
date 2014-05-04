solution "fileos"
	location( "build/" .. _ACTION )
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	startproject "fileos_unittest"

	include "fileos.lua"
	include "fileos_unittest.lua"
	include "../containos/containos_unittest.lua"
	include "../unitos/unitos.lua"