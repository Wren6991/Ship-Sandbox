cmake_minimum_required (VERSION 2.6)

project (ShipSandbox)
 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


####################################################
#                External libraries 
#
# The library paths I'm using are from my MSVC build
# environment; just replace my paths with yours.
# For platforms with no difference between Debug and
# Release builds, just use the same library for both.
####################################################

#
# Picojson
#

set (PICOJSON_SDK_ROOT "C:/Users/Neurodancer/source/SDK/PicoJSON")

set (PICOJSON_INCLUDE_DIRS 
	"${PICOJSON_SDK_ROOT}/include")


#
# WxWidgets
#

set (WXWIDGETS_SDK_ROOT "C:/Users/Neurodancer/source/repos/wxWidgets")

set (WXWIDGETS_INCLUDE_DIRS 
	"${WXWIDGETS_SDK_ROOT}/include" 
	"${WXWIDGETS_SDK_ROOT}/include/msvc"
	"${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib/mswu")

link_directories("${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib")

set (WXWIDGETS_WXMSW_LIBRARY_DEBUG
	"${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib/wxmsw31ud_core.lib")
set (WXWIDGETS_WXBASE_LIBRARY_DEBUG
	"${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib/wxbase31ud.lib")
set (WXWIDGETS_WXMSW_LIBRARY_RELEASE
	"${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib/wxmsw31u_core.lib")
set (WXWIDGETS_WXBASE_LIBRARY_RELEASE
	"${WXWIDGETS_SDK_ROOT}/lib/vc_x64_lib/wxbase31u.lib")


#
# DevIL
#

set (DEVIL_SDK_ROOT "C:/Users/Neurodancer/source/SDK/DevIL")

set (DEVIL_INCLUDE_DIRS "${DEVIL_SDK_ROOT}/include")

set (DEVIL_DEVIL_LIBRARY_DEBUG
	"${DEVIL_SDK_ROOT}/lib/x64/Debug/DevIL.lib")
set (DEVIL_ILU_LIBRARY_DEBUG
	"${DEVIL_SDK_ROOT}/lib/x64/Debug/ILU.lib")
set (DEVIL_DEVIL_LIBRARY_RELEASE
	"${DEVIL_SDK_ROOT}/lib/x64/Release/DevIL.lib")
set (DEVIL_ILU_LIBRARY_RELEASE
	"${DEVIL_SDK_ROOT}/lib/x64/Release/ILU.lib")

set (DEVIL_RUNTIME_LIBRARIES_DEBUG
       "${DEVIL_SDK_ROOT}/lib/x64/Debug/DevIL.dll"
       "${DEVIL_SDK_ROOT}/lib/x64/Debug/ILU.dll")
set (DEVIL_RUNTIME_LIBRARIES_RELEASE
       "${DEVIL_SDK_ROOT}/lib/x64/Release/DevIL.dll"
       "${DEVIL_SDK_ROOT}/lib/x64/Release/ILU.dll")


####################################################
# Flags
####################################################

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_UNICODE /D_CRT_SECURE_NO_WARNINGS /D_HAS_CXX17")

	foreach(flag_var
		CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		if(${flag_var} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		endif(${flag_var} MATCHES "/MD")
		if(${flag_var} MATCHES "/W3")
			string(REGEX REPLACE "/W3" "" ${flag_var} "${${flag_var}}")
		endif(${flag_var} MATCHES "/W3")
		set(${flag_var} "${${flag_var}} /W4")
	endforeach(flag_var)
	
	if(CMAKE_EXE_LINKER_FLAGS_DEBUG MATCHES "/INCREMENTAL")		
		string(REGEX REPLACE "/INCREMENTAL" "" CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
	endif(CMAKE_EXE_LINKER_FLAGS_DEBUG MATCHES "/INCREMENTAL")

	string(REGEX REPLACE "/O2" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /fp:fast /Ox /GS-")

	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
elseif(CMAKE_COMPILER_IS_GNUXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 -O3 -ffast-math -fno-math-errno -funsafe-math-optimizations -ffinite-math-only -fno-trapping-math")
endif()
 


message ("cxx Flags:" ${CMAKE_CXX_FLAGS})
message ("cxx Flags Release:" ${CMAKE_CXX_FLAGS_RELEASE})
message ("cxx Flags Debug:" ${CMAKE_CXX_FLAGS_DEBUG})
message ("exe Linker Flags Debug:" ${CMAKE_EXE_LINKER_FLAGS_DEBUG})



####################################################
# Misc
####################################################

if (MSVC)
	set(ADDITIONAL_LIBRARIES "comctl32;rpcrt4;advapi32") # winmm.lib wsock32.lib
else(MSVC)
	set(ADDITIONAL_LIBRARIES "")
endif(MSVC)


find_package(OpenGL REQUIRED)




####################################################
# Sub-projects
####################################################

add_subdirectory(GameLib)
add_subdirectory(ShipSandbox)
add_subdirectory(UnitTests)




